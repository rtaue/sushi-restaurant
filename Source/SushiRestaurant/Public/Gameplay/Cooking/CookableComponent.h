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

// Delegate for when cooking is finished
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCookingFinishedSignature, AActor*, CookedActor);

/**
 * Component that manages the cooking process of an ingredient.
 * It tracks its state and handles attachment/detachment to stations.
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SUSHIRESTAURANT_API UCookableComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCookableComponent();

	/// Starts the cooking process for the given duration and locks the actor to a station
	void StartCooking(float Duration, AActor* StationActor);

	/// Called when cooking finishes
	UFUNCTION()
	void OnCookingComplete();

	/// Current cooking state (replicated to clients)
	UPROPERTY(ReplicatedUsing = OnRep_CookingState, BlueprintReadOnly)
	ECookingState CookingState;
	
	// Event fired when cooking completes
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FCookingFinishedSignature OnCookingFinished;

	// Result actor when prepared
	UPROPERTY(EditDefaultsOnly, Category = "Cooking")
	TSubclassOf<AActor> ResultActorClass;

	UPROPERTY(EditDefaultsOnly, Category = "Cooking")
	FName ProcessName; // "Chopped", "Boiled", etc.

protected:
	/// Registers replicated properties
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/// Called on clients when CookingState changes
	UFUNCTION()
	void OnRep_CookingState();

private:
	/// Timer handle used for tracking cooking duration
	FTimerHandle CookingTimer;

	/// The station that is currently holding this item
	UPROPERTY()
	AActor* AttachedStation;
};
