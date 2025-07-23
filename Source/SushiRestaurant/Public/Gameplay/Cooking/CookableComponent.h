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
 * Component responsible for managing cooking lifecycle and time tracking.
 * Encapsulates logic for starting, updating, and completing the cooking process.
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SUSHIRESTAURANT_API UCookableComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:
	
	UCookableComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Starts the cooking process */
	void StartCooking(float Duration, AActor* InStation);

	/** Returns true if currently cooking */
	UFUNCTION(BlueprintCallable, Category = "Cooking")
	bool IsCooking() const { return CookingState == ECookingState::Processing; }

	/** Returns true if finished cooking */
	UFUNCTION(BlueprintCallable, Category = "Cooking")
	bool IsCooked() const { return CookingState == ECookingState::Cooked; }

	/** Returns time elapsed since start (for UI/local use) */
	UFUNCTION(BlueprintCallable, Category = "Cooking")
	float GetElapsedTime(const UObject* WorldContext) const;

	/** Returns time remaining until done (for UI/local use) */
	UFUNCTION(BlueprintCallable, Category = "Cooking")
	float GetRemainingTime(const UObject* WorldContext) const;

	/** Called when cooking completes */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FCookingFinishedSignature OnCookingFinished;

	/** Result actor that will replace the cooked one */
	UPROPERTY(EditDefaultsOnly, Category = "Cooking")
	TSubclassOf<AActor> ResultActorClass;

	UPROPERTY(EditDefaultsOnly, Category = "Cooking")
	FName ProcessName;

	/** Current state replicated */
	UPROPERTY(ReplicatedUsing = OnRep_CookingState, BlueprintReadOnly)
	ECookingState CookingState;

protected:

	UFUNCTION()
	void OnRep_CookingState() const;

private:

	/** Time when cooking started (only relevant during Processing) */
	UPROPERTY()
	float CookingStartTime = 0.f;

	/** Duration for the cooking process */
	UPROPERTY()
	float CookingDuration = 0.f;

	/** Timer handle used by the server */
	FTimerHandle CookingTimer;

	/** Station to which the item is attached (for reference) */
	UPROPERTY()
	AActor* AttachedStation;

	/** Called on server when cooking finishes */
	void OnCookingComplete();
};
