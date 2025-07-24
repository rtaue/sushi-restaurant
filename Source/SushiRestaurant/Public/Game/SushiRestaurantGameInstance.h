// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "SushiRestaurantGameInstance.generated.h"

/**
 * Manages hosting and joining sessions for multiplayer
 */
UCLASS()
class SUSHIRESTAURANT_API USushiRestaurantGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	// Hosts the game as listen server
	UFUNCTION(BlueprintCallable)
	void HostGame() const;

	// Joins a game at a predefined IP
	UFUNCTION(BlueprintCallable)
	void JoinGame() const;

	// Starts the actual gameplay (from lobby/map menu)
	UFUNCTION(BlueprintCallable)
	void StartGame() const;
};
