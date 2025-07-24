// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/Cooking/StationBase.h"
#include "DispenserStation.generated.h"

/**
 * 
 */
UCLASS()
class SUSHIRESTAURANT_API ADispenserStation : public AStationBase
{
	GENERATED_BODY()

public:
	
	virtual void Interact_Implementation(AActor* Interactor) override;

protected:
	// What ingredient this station dispenses
	UPROPERTY(EditAnywhere, Category="Dispenser")
	TSubclassOf<AActor> IngredientToSpawn;
	
};
