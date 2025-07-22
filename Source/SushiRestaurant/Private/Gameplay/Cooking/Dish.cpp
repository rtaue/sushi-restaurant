// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Cooking/Dish.h"

// Sets default values
ADish::ADish()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	DishMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DishMesh"));
	RootComponent = DishMeshComponent;
}

