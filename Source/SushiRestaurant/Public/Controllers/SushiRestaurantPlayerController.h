// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SushiRestaurantPlayerController.generated.h"

class UScoreWidget;
struct FInputActionValue;
class UInputAction;
class UInputMappingContext;

/**
 *  Basic PlayerController class for a third person game
 *  Manages input mappings
 */
UCLASS(abstract)
class ASushiRestaurantPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:

	virtual void BeginPlay() override;

	/** Input Mapping Contexts */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category ="Input", meta = (AllowPrivateAccess = "true"))
	TArray<UInputMappingContext*> DefaultMappingContexts;

	/** Input mapping context setup */
	virtual void SetupInputComponent() override;

	void PerformInteractionTrace();

	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputAction* InteractAction;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category ="Interaction", meta = (AllowPrivateAccess = "true"))
	float InteractionRange = 200.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category ="Interaction", meta = (AllowPrivateAccess = "true"))
	float InteractionCapsuleRadius = 50.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category ="Interaction", meta = (AllowPrivateAccess = "true"))
	float InteractionCapsuleHalfHeight = 75.f;

	void OnInteract(const FInputActionValue& Value);

	UFUNCTION(Server, Reliable)
	void Server_OnInteract();

#pragma region Widgets

	/** Widget class used to display the score */
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UScoreWidget> ScoreWidgetClass;

	/** Runtime instance of the score display */
	UPROPERTY()
	UScoreWidget* ScoreWidget;
	
	/** Respond to score update events */
	UFUNCTION()
	void HandleScoreUpdated(float NewScore);

	/** Adds all gameplay widgets to the screen */
	void CreateGameplayWidgets();

#pragma endregion
};


