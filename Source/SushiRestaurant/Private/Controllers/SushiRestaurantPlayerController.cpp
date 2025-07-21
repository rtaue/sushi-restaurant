// Copyright Epic Games, Inc. All Rights Reserved.


#include "Controllers/SushiRestaurantPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "Interfaces/InteractableInterface.h"
#include "SushiRestaurant/SushiRestaurant.h"

void ASushiRestaurantPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Add Input Mapping Contexts
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
		{
			Subsystem->AddMappingContext(CurrentContext, 0);
		}
	}

	if (UEnhancedInputComponent* Input = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		Input->BindAction(InteractAction, ETriggerEvent::Triggered, this, &ASushiRestaurantPlayerController::OnInteract);
	}

}

void ASushiRestaurantPlayerController::PerformInteractionTrace()
{
	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn) return;

	const FVector Start = ControlledPawn->GetActorLocation();
	const FVector Forward = ControlledPawn->GetActorForwardVector();
	const FVector End = Start + Forward * 200.f;

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(ControlledPawn);

	// Draw line for debug
	DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 1.f, 0, 2.f);

	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
	{
		if (AActor* HitActor = Hit.GetActor())
		{
			UE_LOG(LogSushiInteraction, Log, TEXT("Trace hit actor: %s"), *HitActor->GetName());

			if (HitActor->Implements<UInteractableInterface>())
			{
				UE_LOG(LogSushiInteraction, Log, TEXT("Actor %s implements IInteractableInterface"), *HitActor->GetName());

				IInteractableInterface::Execute_Interact(HitActor, ControlledPawn);
			}
			else
			{
				UE_LOG(LogSushiInteraction, Warning, TEXT("Actor %s is not interactable"), *HitActor->GetName());
			}
		}
	}
	else
	{
		UE_LOG(LogSushiInteraction, Verbose, TEXT("Nothing hit by interaction trace."));
	}
}

void ASushiRestaurantPlayerController::OnInteract(const FInputActionValue& Value)
{
	if (IsLocalController())
	{
		UE_LOG(LogSushiInteraction, Log, TEXT("OnInteract triggered locally"));
		Server_OnInteract();
	}
}

void ASushiRestaurantPlayerController::Server_OnInteract_Implementation()
{
	UE_LOG(LogSushiInteraction, Log, TEXT("Server_OnInteract executed"));
	
	PerformInteractionTrace();
}
