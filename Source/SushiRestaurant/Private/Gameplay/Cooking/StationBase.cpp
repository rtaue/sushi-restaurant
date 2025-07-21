// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Cooking/StationBase.h"

#include "SushiRestaurant/SushiRestaurant.h"

// Sets default values
AStationBase::AStationBase()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
}

void AStationBase::Interact_Implementation(AActor* Interactor)
{
	UE_LOG(LogSushiInteraction, Warning, TEXT("%s interacted with %s"), *Interactor->GetName(), *GetName());
}

