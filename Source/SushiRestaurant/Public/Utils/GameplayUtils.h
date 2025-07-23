// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameplayUtils.generated.h"

/**
 * 
 */
UCLASS()
class SUSHIRESTAURANT_API UGameplayUtils : public UObject
{
	GENERATED_BODY()

public:

	// Returns true if the local player controller is valid and locally controlled
	UFUNCTION(BlueprintPure, Category = "Gameplay|Utils")
	static bool IsLocallyControlled(const UWorld* World);
};
