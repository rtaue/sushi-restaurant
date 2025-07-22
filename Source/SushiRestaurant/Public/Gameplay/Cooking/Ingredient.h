// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/RecipeSourceInterface.h"
#include "Ingredient.generated.h"

UCLASS()
class SUSHIRESTAURANT_API AIngredient : public AActor, public IRecipeSourceInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AIngredient();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	URecipeData* Recipe;

	virtual URecipeData* GetRecipe_Implementation() const override { return Recipe; };

protected:
	
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* IngredientMeshComponent;
};
