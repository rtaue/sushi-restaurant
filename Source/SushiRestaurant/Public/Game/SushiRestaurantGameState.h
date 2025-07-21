// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "SushiRestaurantGameState.generated.h"

/**
 * 
 */
UCLASS()
class SUSHIRESTAURANT_API ASushiRestaurantGameState : public AGameStateBase
{
	GENERATED_BODY()

public:

	ASushiRestaurantGameState();

	UPROPERTY(Replicated, BlueprintReadOnly, Category="Game")
	float RemainingTime;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
