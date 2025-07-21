// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CookableComponent.generated.h"

/// Represents the current state of an ingredient during preparation
UENUM(BlueprintType)
enum class ECookingState : uint8
{
	Raw         UMETA(DisplayName = "Raw"),         // Not processed yet
	Processing  UMETA(DisplayName = "Processing"),  // Being processed (e.g., sliced, cooked)
	Cooked      UMETA(DisplayName = "Cooked")       // Fully prepared
};

/**
 * Component that handles the cooking state and timing for ingredients.
 * Can be attached to any actor representing a cookable item.
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SUSHIRESTAURANT_API UCookableComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCookableComponent();

	/// Starts the cooking process for a specified duration (only on server)
	void StartCooking(float Duration);

	/// Called when cooking finishes
	UFUNCTION()
	void OnCookingComplete();

	/// Current cooking state (replicated to clients)
	UPROPERTY(ReplicatedUsing = OnRep_CookingState, BlueprintReadOnly)
	ECookingState CookingState;

protected:
	/// Registers replicated properties
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/// Called on clients when CookingState changes
	UFUNCTION()
	void OnRep_CookingState();

private:
	/// Timer handle used for tracking cooking duration
	FTimerHandle CookingTimer;
		
};
