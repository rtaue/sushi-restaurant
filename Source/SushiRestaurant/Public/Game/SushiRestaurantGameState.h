// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "SushiRestaurantGameState.generated.h"

class UScoreWidget;
class URecipeData;
struct FActiveOrder;

// Delegate for score changes
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnScoreChangedSignature, float, NewScore);

/**
 * GameState for handling shared team state such as score in cooperative mode.
 */
UCLASS()
class SUSHIRESTAURANT_API ASushiRestaurantGameState : public AGameStateBase
{
	GENERATED_BODY()

public:

	ASushiRestaurantGameState();

protected:
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	
	// Returns current score
	UFUNCTION(BlueprintCallable, Category = "Score")
	int32 GetCurrentScore() const { return CurrentScore; }

	// Called when a dish is delivered (but not necessarily the order completed)
	void HandleRecipeDelivered(const URecipeData* Recipe, float TimeRemaining, float TotalTime);

	// Called when a full order is completed
	void HandleOrderCompleted(const FActiveOrder& Order);

	// Called when an order fails (e.g., time ran out)
	void HandleOrderFailed(const FActiveOrder& Order);

	// Event for listening to score updates (e.g., HUD)
	UPROPERTY(BlueprintAssignable, Category = "Score")
	FOnScoreChangedSignature OnScoreChanged;

protected:

	// Cooperative score across all players
	UPROPERTY(ReplicatedUsing = OnRep_CurrentScore, VisibleAnywhere, BlueprintReadOnly, Category = "Score")
	int32 CurrentScore = 0;

	UFUNCTION()
	void OnRep_CurrentScore() const;

	// Multiplier for early deliveries (optional)
	UPROPERTY(EditDefaultsOnly, Category = "Score")
	float TimeBonusMultiplier = 1.0f;

	// Points lost for failed order
	UPROPERTY(EditDefaultsOnly, Category = "Score")
	int32 PenaltyOnFail = 50;

	UPROPERTY(Replicated)
	int RemainingTime;

};
