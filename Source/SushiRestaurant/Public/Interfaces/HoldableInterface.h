// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HoldableInterface.generated.h"

/**
 * Interface for items that can be picked up and dropped by players.
 * Classes that implement this interface should define behavior for when the item is held or released.
 */
UINTERFACE()
class UHoldableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * This interface provides a contract for holdable items,
 * such as tools, ingredients, or any interactive object that can be carried.
 */
class SUSHIRESTAURANT_API IHoldableInterface
{
	GENERATED_BODY()

public:
	/** Called when the item is picked up by a player */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Holdable")
	void OnPickedUp();

	/** Called when the item is dropped by a player */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Holdable")
	void OnDropped();
};
