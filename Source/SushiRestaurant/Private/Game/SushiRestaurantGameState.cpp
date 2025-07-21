// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/SushiRestaurantGameState.h"

#include "Net/UnrealNetwork.h"

ASushiRestaurantGameState::ASushiRestaurantGameState()
{
	RemainingTime = 120.f; // default 2 min
}

void ASushiRestaurantGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ThisClass, RemainingTime);
}
