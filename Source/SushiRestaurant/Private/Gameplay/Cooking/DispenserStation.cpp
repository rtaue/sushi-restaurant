// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Cooking/DispenserStation.h"

#include "Characters/SushiRestaurantCharacter.h"

void ADispenserStation::Interact_Implementation(AActor* Interactor)
{
	if (ASushiRestaurantCharacter* Character = Cast<ASushiRestaurantCharacter>(Interactor))
	{
		if (!Character->IsHoldingItem() && IngredientToSpawn)
		{
			FVector SpawnLocation = Character->GetActorLocation() + FVector(0, 0, 50);
			FActorSpawnParameters Params;

			AActor* NewIngredient = GetWorld()->SpawnActor<AActor>(IngredientToSpawn, SpawnLocation, FRotator::ZeroRotator, Params);
			Character->PickupItem(NewIngredient);
		}
	}
}
