// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "RecipeSourceInterface.generated.h"

class URecipeData;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class URecipeSourceInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SUSHIRESTAURANT_API IRecipeSourceInterface
{
	GENERATED_BODY()

public:
	// Should return the recipe that this actor represents
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Recipe")
	URecipeData* GetRecipe() const;
};
