// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/SushiRestaurantGameState.h"

#include "Gameplay/Order/ActiveOrder.h"
#include "Net/UnrealNetwork.h"
#include "SushiRestaurant/SushiRestaurant.h"
#include "Widgets/Score/ScoreWidget.h"

ASushiRestaurantGameState::ASushiRestaurantGameState()
{
	// Enable replication for score
	bReplicates = true;

	CurrentScore = 0;
	RemainingTime = 120.f; // default 2 min
}

void ASushiRestaurantGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, CurrentScore);
	DOREPLIFETIME(ThisClass, RemainingTime);
}

void ASushiRestaurantGameState::HandleRecipeDelivered(const URecipeData* Recipe, const float TimeRemaining, const float TotalTime)
{
	if (!HasAuthority() || !Recipe) return;

	const int32 BasePoints = Recipe->BaseScore;
	const float TimeFactor = FMath::Clamp(TimeRemaining / TotalTime, 0.f, 1.f);
	const int32 Bonus = FMath::RoundToInt(BasePoints * TimeFactor * TimeBonusMultiplier);

	const int32 Total = BasePoints + Bonus;
	CurrentScore += Total;

	OnScoreChanged.Broadcast(CurrentScore);

	UE_LOG(LogSushiRestaurantGameState, Log, TEXT("Delivered %s. Base: %d, Bonus: %d, Total: %d"),
		*Recipe->GetName(), BasePoints, Bonus, Total);
}

void ASushiRestaurantGameState::HandleOrderCompleted(const FActiveOrder& Order)
{
	if (!HasAuthority()) return;

	constexpr int32 OrderCompletionBonus = 100;
	CurrentScore += OrderCompletionBonus;

	OnScoreChanged.Broadcast(CurrentScore);

	UE_LOG(LogSushiRestaurantGameState, Log, TEXT("Order completed. +%d points"), OrderCompletionBonus);
}

void ASushiRestaurantGameState::HandleOrderFailed(const FActiveOrder& Order)
{
	if (!HasAuthority())
		return;

	// Example: penalty per undelivered recipe
	int32 MissingDishes = 0;

	for (const FRecipeCount& Req : Order.RequiredRecipes)
	{
		const int32 Delivered = Order.GetDeliveredCountFor(Req.Recipe);
		MissingDishes += FMath::Max(Req.Count - Delivered, 0);
	}

	const int32 Penalty = MissingDishes * PenaltyOnFail;
	CurrentScore += Penalty;

	OnScoreChanged.Broadcast(CurrentScore);

	UE_LOG(LogSushiRestaurantGameState, Warning, TEXT("[SCORE] Order failed. Missing %d dishes. Penalty: -%d"),
		MissingDishes, Penalty);
}

void ASushiRestaurantGameState::OnRep_CurrentScore() const
{
	UE_LOG(LogSushiRestaurantGameState, Log, TEXT("[SCORE] Score updated: %.2d"), CurrentScore);
	OnScoreChanged.Broadcast(CurrentScore);
}

