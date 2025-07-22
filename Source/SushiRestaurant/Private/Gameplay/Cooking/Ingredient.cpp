// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Cooking/Ingredient.h"

// Sets default values
AIngredient::AIngredient()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	IngredientMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("IngredientMesh"));
	RootComponent = IngredientMeshComponent;
}

