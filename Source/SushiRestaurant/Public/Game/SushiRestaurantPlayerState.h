// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "SushiRestaurantPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class SUSHIRESTAURANT_API ASushiRestaurantPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	
	ASushiRestaurantPlayerState();

	UPROPERTY(Replicated, BlueprintReadOnly, Category="Score")
	int32 PlayerScore;

	void AddScore(int32 Points);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
