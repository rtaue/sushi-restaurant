// Copyright Epic Games, Inc. All Rights Reserved.


#include "Controllers/SushiRestaurantPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "Blueprint/UserWidget.h"
#include "Game/SushiRestaurantGameState.h"
#include "Interfaces/InteractableInterface.h"
#include "SushiRestaurant/SushiRestaurant.h"
#include "Widgets/Score/ScoreWidget.h"

void ASushiRestaurantPlayerController::BeginPlay()
{
	Super::BeginPlay();

	CreateGameplayWidgets();

	SetInputMode(FInputModeGameOnly());
	bShowMouseCursor = false;
}

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
	const FVector ForwardVector = ControlledPawn->GetActorForwardVector();
	const FVector End = Start + ForwardVector * InteractionRange; // Interaction radius

	// Collision params
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(ControlledPawn);

	// Defines collision shape
	const FCollisionShape CollisionShape = FCollisionShape::MakeCapsule(InteractionCapsuleRadius, InteractionCapsuleHalfHeight);

	// Draw debug
	DrawDebugCapsule(GetWorld(), (Start + End) / 2, InteractionCapsuleHalfHeight, InteractionCapsuleRadius, FRotationMatrix::MakeFromZ(End - Start).ToQuat(), FColor::Green, false, 1.f, 0, 2.f);

	// Sweep
	FHitResult Hit;
	if (GetWorld()->SweepSingleByChannel(Hit, Start, End, FQuat::Identity, ECC_Visibility, CollisionShape, Params))
	{
		if (AActor* HitActor = Hit.GetActor())
		{
			UE_LOG(LogSushiInteraction, Log, TEXT("Capsule sweep hit actor: %s"), *HitActor->GetName());

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
		UE_LOG(LogSushiInteraction, Verbose, TEXT("Nothing hit by interaction capsule sweep."));
	}
}

void ASushiRestaurantPlayerController::OnInteract(const FInputActionValue& Value)
{
	if (IsLocalController())
	{
		UE_LOG(LogSushiInteraction, Verbose, TEXT("OnInteract triggered locally"));

		// Avoid spamming
		if (!IsPaused())
		{
			Server_OnInteract();
		}
	}
}

void ASushiRestaurantPlayerController::Server_OnInteract_Implementation()
{
	UE_LOG(LogSushiInteraction, Verbose, TEXT("Server_OnInteract executed"));

	if (const APawn* ControlledPawn = GetPawn(); !ControlledPawn)
	{
		UE_LOG(LogSushiInteraction, Warning, TEXT("No controlled pawn to interact with"));
		return;
	}
	
	PerformInteractionTrace();
}

void ASushiRestaurantPlayerController::HandleScoreUpdated(const float NewScore)
{
	if (ScoreWidget)
	{
		ScoreWidget->UpdateScore(NewScore);
	}
}

void ASushiRestaurantPlayerController::CreateGameplayWidgets()
{
	// Spawn and add the score widget
	if (ScoreWidgetClass)
	{
		ScoreWidget = CreateWidget<UScoreWidget>(GetWorld(), ScoreWidgetClass);
		if (ScoreWidget)
		{
			ScoreWidget->AddToViewport();

			// Bind to score changes from GameState
			if (ASushiRestaurantGameState* SushiGS = GetWorld()->GetGameState<ASushiRestaurantGameState>())
			{
				SushiGS->OnScoreChanged.AddDynamic(this, &ASushiRestaurantPlayerController::HandleScoreUpdated);

				// Initialize widget with current score
				ScoreWidget->UpdateScore(SushiGS->GetCurrentScore());
			}
		}
	}
}
