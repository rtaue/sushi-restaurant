// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OrderManager.generated.h"

class ADeliveryStation;
class URecipeData;
struct FActiveOrder;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnOrdersUpdated);

UCLASS()
class SUSHIRESTAURANT_API AOrderManager : public AActor
{
	GENERATED_BODY()

public:
	AOrderManager();

	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;

	// Assign a dish to the correct delivery station
	UFUNCTION(BlueprintCallable)
	void AssignOrdersToStations();

	// List of all available recipes to use in generation
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<URecipeData*> AllRecipes;

	// Max number of recipes of an active order
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	uint8 MaxNumOfOrderRecipes = 3;

	// Automatically found or manually assigned delivery stations
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<ADeliveryStation*> DeliveryStations;

	// Broadcast whenever order states are updated
	UPROPERTY(BlueprintAssignable)
	FOnOrdersUpdated OnOrdersUpdated;

	// Interval to check for empty tables
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Orders")
	float StationCheckInterval = 5.f;

private:
	float TimeUntilStationCheck = 0.f;

};
