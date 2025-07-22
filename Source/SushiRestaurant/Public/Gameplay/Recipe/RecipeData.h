// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "RecipeData.generated.h"

/**
 * 
 */
UCLASS()
class SUSHIRESTAURANT_API URecipeData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	
	// Name of the dish shown in UI
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Recipe")
	FText DishName;

	// Final dish actor that should be delivered
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Recipe")
	TSubclassOf<AActor> FinalDishClass;

	// List of required ingredients (can be intermediate items too)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Recipe")
	TArray<TSubclassOf<AActor>> RequiredIngredients;

	// Optional: Expected preparation steps (for validation or UI)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Recipe")
	TArray<FName> RequiredProcesses;
	
};
