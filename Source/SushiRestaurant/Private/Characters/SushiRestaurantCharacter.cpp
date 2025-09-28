// Copyright Epic Games, Inc. All Rights Reserved.

#include "Characters/SushiRestaurantCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Gameplay/Equip/EquippedVisualData.h"
#include "Interfaces/HoldableInterface.h"
#include "Net/UnrealNetwork.h"
#include "SushiRestaurant/SushiRestaurant.h"

ASushiRestaurantCharacter::ASushiRestaurantCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	// --- Inventory & Interaction Setup ---

	// Attach point for held items like plates or ingredients.
	ItemAttachPoint = CreateDefaultSubobject<USceneComponent>(TEXT("ItemAttachPoint"));
	ItemAttachPoint->SetupAttachment(GetMesh(), FName("ItemSocket"));
	ItemAttachPoint->SetRelativeLocation(FVector(50.f, 0.f, 50.f)); 

	// Component to display visual equipment like a knife or pan.
	EquipVisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EquipVisualMesh"));
	EquipVisualMesh->SetupAttachment(GetMesh(), FName("EquipSocket"));
	EquipVisualMesh->SetIsReplicated(false);
	EquipVisualMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	EquipVisualMesh->SetVisibility(false);
}

void ASushiRestaurantCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, HeldItem);
	DOREPLIFETIME(ThisClass, EquippedVisual);
}

void ASushiRestaurantCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASushiRestaurantCharacter::Move);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &ASushiRestaurantCharacter::Look);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASushiRestaurantCharacter::Look);
	}
	else
	{
		UE_LOG(LogSushiRestaurantCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ASushiRestaurantCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	// route the input
	DoMove(MovementVector.X, MovementVector.Y);
}

void ASushiRestaurantCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// route the input
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void ASushiRestaurantCharacter::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
	}
}

void ASushiRestaurantCharacter::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void ASushiRestaurantCharacter::DoJumpStart()
{
	// signal the character to jump
	Jump();
}

void ASushiRestaurantCharacter::DoJumpEnd()
{
	// signal the character to stop jumping
	StopJumping();
}

#pragma region Inventory and Interaction Implementation

void ASushiRestaurantCharacter::PickupItem(AActor* Item)
{
    // This function changes game state and should only ever run on the server.
    if (!HasAuthority())
    {
        return;
    }

    if (!Item || HeldItem)
    {
        UE_LOG(LogSushiRestaurantCharacter, Warning, TEXT("PickupItem failed: Invalid item or character already holding an item."));
        return;
    }

    if (Item->Implements<UHoldableInterface>())
    {
        HeldItem = Item;
    	
    	// Execute OnPickedUp behavior (e.g. disable physics)
    	IHoldableInterface::Execute_OnPickedUp(Item);

    	// Attach the item to the character's socket, preserving socket scale
    	Item->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, "ItemSocket");

    	// Force scale in case of inconsistent replication
    	Item->SetActorScale3D(FVector(1.0f));

    	// Optional: disable movement replication while held
    	Item->SetReplicateMovement(false); 
        
        UE_LOG(LogSushiRestaurantCharacter, Log, TEXT("Picked up item: %s on server."), *Item->GetName());
    }
}

void ASushiRestaurantCharacter::DropItem()
{
    // This function changes game state and should only ever run on the server.
    if (!HasAuthority() || !HeldItem)
    {
        return;
    }

    if (HeldItem->Implements<UHoldableInterface>())
    {
        // Detach on the server. OnRep will handle detachment on clients when HeldItem becomes null.
        HeldItem->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
        HeldItem->SetReplicateMovement(true);
       
        IHoldableInterface::Execute_OnDropped(HeldItem);

        UE_LOG(LogSushiRestaurantCharacter, Log, TEXT("Dropped item: %s on server."), *HeldItem->GetName());
        
        HeldItem = nullptr;
        // OnRep will be automatically triggered for clients due to the variable change.
    }
}

void ASushiRestaurantCharacter::OnRep_HeldItem()
{
    if (HeldItem && HeldItem->Implements<UHoldableInterface>())
    {
        // This runs on clients to visually attach the item.
        IHoldableInterface::Execute_OnPickedUp(HeldItem); // Disables physics, etc.
        HeldItem->AttachToComponent(ItemAttachPoint, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("ItemSocket"));
    	HeldItem->SetActorScale3D(FVector(1.0f));
        HeldItem->SetReplicateMovement(false); // Movement is now controlled by the character.
    }
    // If HeldItem is now null, the logic to detach is implicitly handled by the engine's replication system.
}

void ASushiRestaurantCharacter::LockToStation(AActor* Station)
{
    // Server-only logic.
    if (!HasAuthority() || !Station || LockedStation) return;

    LockedStation = Station;
    GetCharacterMovement()->DisableMovement();
    
    // Controller logic is fine here as it's server-authoritative.
    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
       PC->SetIgnoreMoveInput(true);
    }
    UE_LOG(LogSushiRestaurantCharacter, Log, TEXT("Character %s locked to station %s."), *GetName(), *Station->GetName());
}

void ASushiRestaurantCharacter::UnlockFromStation()
{
    // Server-only logic.
    if (!HasAuthority() || !LockedStation) return;
    
    UE_LOG(LogSushiRestaurantCharacter, Log, TEXT("Character %s unlocked from station %s."), *GetName(), *LockedStation->GetName());

    LockedStation = nullptr;
    GetCharacterMovement()->SetMovementMode(MOVE_Walking);

    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
       PC->SetIgnoreMoveInput(false);
    }
}

// --- Interaction RPC Flow ---

void ASushiRestaurantCharacter::RequestInteraction(UAnimMontage* Montage, const EEquippedVisual& Equip)
{
    // Client requests the interaction from the server. The server executes it directly.
    Server_SetInteractionState(Montage, Equip);
}

void ASushiRestaurantCharacter::RequestStopInteraction()
{
    // Client requests to stop from the server.
    Server_ClearInteractionState();
}

void ASushiRestaurantCharacter::Server_SetInteractionState_Implementation(UAnimMontage* Montage, EEquippedVisual Equip)
{
    // The server is the authority. It sets the state and then tells clients.
    EquippedVisual = Equip;

    // Manually call OnRep on the server because it doesn't trigger automatically.
    OnRep_EquippedVisual();
    
    // Tell all clients to play the montage.
    Multicast_PlayMontage(Montage);

    UE_LOG(LogSushiRestaurantCharacter, Log, TEXT("Server: Set interaction state with Montage: %s"), *GetNameSafe(Montage));
}

void ASushiRestaurantCharacter::Server_ClearInteractionState_Implementation()
{
    // Server clears the state.
    EquippedVisual = EEquippedVisual::None;

    // Manually call OnRep on the server.
    OnRep_EquippedVisual();
    
    // Tell all clients to stop the montage.
    Multicast_StopMontage();
    
    UE_LOG(LogSushiRestaurantCharacter, Log, TEXT("Server: Cleared interaction state."));
}

void ASushiRestaurantCharacter::Multicast_PlayMontage_Implementation(UAnimMontage* Montage)
{
    if (Montage && GetMesh() && GetMesh()->GetAnimInstance())
    {
       GetMesh()->GetAnimInstance()->Montage_Play(Montage);
    }
}

void ASushiRestaurantCharacter::Multicast_StopMontage_Implementation()
{
    if (GetMesh() && GetMesh()->GetAnimInstance())
    {
       GetMesh()->GetAnimInstance()->Montage_Stop(0.2f);
    }
}

// --- Visual Update Logic ---

void ASushiRestaurantCharacter::OnRep_EquippedVisual()
{
    // This is called on all clients when the 'EquippedVisual' variable is replicated.
    UpdateEquippedVisual();
}

void ASushiRestaurantCharacter::UpdateEquippedVisual()
{
    // Safety check for the component. It should always be valid if created in the constructor.
    if (!EquipVisualMesh)
    {
        UE_LOG(LogSushiRestaurantCharacter, Error, TEXT("EquipVisualMesh is NULL on character %s!"), *GetName());
        return;
    }

    if (EquippedVisual == EEquippedVisual::None)
    {
       // Hide the mesh if no item is equipped.
       EquipVisualMesh->SetVisibility(false);
       EquipVisualMesh->SetStaticMesh(nullptr);
       return;
    }

    // EFFICIENT LOOKUP: Find the correct row in the DataTable.
    if (EquippedVisualTable)
    {
        // Convert the enum to a string to use as the row name.
    	const FString EnumAsString = StaticEnum<EEquippedVisual>()->GetNameStringByValue(static_cast<int64>(EquippedVisual));
        const FName RowName = FName(*EnumAsString);

        // FindRow is significantly faster than iterating all rows.
        const FEquippedVisualData* RowData = EquippedVisualTable->FindRow<FEquippedVisualData>(RowName, TEXT("EquippedVisualLookup"));

        if (RowData && RowData->Mesh)
        {
            EquipVisualMesh->SetStaticMesh(RowData->Mesh);
            EquipVisualMesh->SetVisibility(true);
            UE_LOG(LogSushiRestaurantCharacter, Verbose, TEXT("Updated equipped visual to %s"), *RowName.ToString());
        }
        else
        {
            UE_LOG(LogSushiRestaurantCharacter, Warning, TEXT("Could not find a valid mesh for visual type '%s' in the DataTable."), *RowName.ToString());
            EquipVisualMesh->SetVisibility(false);
        }
    }
}

#pragma endregion

