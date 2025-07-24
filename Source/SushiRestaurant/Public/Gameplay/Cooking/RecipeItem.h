// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableItem.h"
#include "Interfaces/RecipeSourceInterface.h"
#include "RecipeItem.generated.h"

UCLASS()
class SUSHIRESTAURANT_API ARecipeItem : public AInteractableItem, public IRecipeSourceInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ARecipeItem();

	virtual URecipeData* GetRecipe_Implementation() const override { return Recipe; };

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Recipe")
	URecipeData* Recipe;
};
