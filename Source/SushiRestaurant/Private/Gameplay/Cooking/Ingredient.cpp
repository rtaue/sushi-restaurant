// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Cooking/Ingredient.h"

// Sets default values
AIngredient::AIngredient()
{
	CookableComponent = CreateDefaultSubobject<UCookableComponent>("CookableComponent");
}

