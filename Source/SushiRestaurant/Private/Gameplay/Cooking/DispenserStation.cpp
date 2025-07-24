// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Cooking/DispenserStation.h"

#include "Characters/SushiRestaurantCharacter.h"

void ADispenserStation::Interact_Implementation(AActor* Interactor)
{
	if (ASushiRestaurantCharacter* Character = Cast<ASushiRestaurantCharacter>(Interactor))
	{
		if (!Character->IsHoldingItem() && IngredientToSpawn)
		{
			const FVector SpawnLocation = Character->GetActorLocation() + FVector(0, 0, 50);
			
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = GetInstigator();
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			AActor* NewIngredient = GetWorld()->SpawnActor<AActor>(IngredientToSpawn, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
			// Character->PickupItem(NewIngredient);
			// Temp fix for client replication race
			FTimerHandle DelayHandle;
			GetWorldTimerManager().SetTimer(DelayHandle, FTimerDelegate::CreateLambda([=]()
			{
				if (IsValid(Character) && IsValid(NewIngredient))
				{
					Character->PickupItem(NewIngredient);
				}
			}), 0.1f, false);
		}
	}
}
