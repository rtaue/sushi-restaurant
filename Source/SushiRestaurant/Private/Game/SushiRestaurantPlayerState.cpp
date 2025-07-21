// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/SushiRestaurantPlayerState.h"

#include "Net/UnrealNetwork.h"

ASushiRestaurantPlayerState::ASushiRestaurantPlayerState()
{
	PlayerScore = 0;
}

void ASushiRestaurantPlayerState::AddScore(int32 Points)
{
	PlayerScore += Points;
}

void ASushiRestaurantPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ThisClass, PlayerScore);
}
