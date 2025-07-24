// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CookableComponent.h"
#include "RecipeItem.h"
#include "GameFramework/Actor.h"
#include "Ingredient.generated.h"

UCLASS()
class SUSHIRESTAURANT_API AIngredient : public ARecipeItem
{
	GENERATED_BODY()
	
public:
	
	// Sets default values for this actor's properties
	AIngredient();

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UCookableComponent* CookableComponent;
	
};
