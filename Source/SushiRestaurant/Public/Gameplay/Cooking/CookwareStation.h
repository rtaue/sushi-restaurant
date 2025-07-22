// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/Cooking/StationBase.h"
#include "CookwareStation.generated.h"

class ASushiRestaurantCharacter;
/**
 * Cookware station that processes ingredients with a CookableComponent.
 */
UCLASS()
class SUSHIRESTAURANT_API ACookwareStation : public AStationBase
{
	GENERATED_BODY()

public:

	ACookwareStation();
	
	virtual void Interact_Implementation(AActor* Interactor) override;

protected:
	// Cooking time in seconds
	UPROPERTY(EditAnywhere, Category="Cooking")
	float CookingTime = 3.0f;

	// Accepted ingredient type for this station
	UPROPERTY(EditAnywhere, Category="Cooking")
	TSubclassOf<AActor> AcceptedIngredientClass;

	// Anchor point where the ingredient will appear during processing
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cooking")
	USceneComponent* IngredientAnchor;
	
	// The character currently using this station
	UPROPERTY()
	ASushiRestaurantCharacter* CurrentUser;

	// The item currently at the station
	UPROPERTY()
	AActor* CurrentHeldItem;

	UFUNCTION()
	void OnIngredientCooked(AActor* CookedActor);
};
