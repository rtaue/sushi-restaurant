// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Cooking/CookwareStation.h"

#include "Characters/SushiRestaurantCharacter.h"
#include "Gameplay/Cooking/CookableComponent.h"
#include "SushiRestaurant/SushiRestaurant.h"

ACookwareStation::ACookwareStation()
{
	IngredientAnchor = CreateDefaultSubobject<USceneComponent>(TEXT("IngredientAnchor"));
	IngredientAnchor->SetupAttachment(RootComponent);
}

void ACookwareStation::Interact_Implementation(AActor* Interactor)
{
	// If already in use, reject
	if (CurrentUser)
	{
		UE_LOG(LogCookwareStation, Warning, TEXT("Station %s is already in use by %s"),
			*GetName(), *CurrentUser->GetName());
		return;
	}

	// Proceed with interaction
	if (ASushiRestaurantCharacter* Character = Cast<ASushiRestaurantCharacter>(Interactor))
	{
		if (Character->IsHoldingItem())
		{
			AActor* Item = Character->HeldItem;

			// Check if the ingredient is valid for this station
			if (!AcceptedIngredientClass || !Item->IsA(AcceptedIngredientClass))
			{
				UE_LOG(LogCookwareStation, Warning, TEXT("Rejected item %s"), *Item->GetName());
				return;
			}

			if (UCookableComponent* Cookable = Item->FindComponentByClass<UCookableComponent>())
			{
				Character->DropItem();
				if (Item)
				{
					// Disable physics and collision
					if (UPrimitiveComponent* Root = Cast<UPrimitiveComponent>(Item->GetRootComponent()))
					{
						Root->SetSimulatePhysics(false);
						Item->SetActorEnableCollision(false);
					}

					// Attach to station's anchor point
					Item->AttachToComponent(IngredientAnchor, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

					CurrentHeldItem = Item;
				}
				
				Character->LockToStation(this);

				CurrentUser = Character;

				// Register unlock callback
				Cookable->OnCookingFinished.AddDynamic(this, &ACookwareStation::OnIngredientCooked);
				Cookable->StartCooking(CookingTime, this);
			}
		}
		else if (CurrentHeldItem)
		{
			Character->PickupItem(CurrentHeldItem);
			CurrentHeldItem = nullptr;
		}
	}
}

void ACookwareStation::OnIngredientCooked(AActor* CookedActor)
{
	if (const UCookableComponent* Cookable = CookedActor->FindComponentByClass<UCookableComponent>())
	{
		// Spawn the next stage of the ingredient
		if (Cookable->ResultActorClass)
		{
			const FVector SpawnLocation = CookedActor->GetActorLocation();
			const FRotator SpawnRotation = CookedActor->GetActorRotation();
            
			// Destroy current
			CookedActor->Destroy();

			// Spawn new processed actor
			AActor* NewResult = GetWorld()->SpawnActor<AActor>(
				Cookable->ResultActorClass,
				SpawnLocation,
				SpawnRotation
			);

			// Attach to station
			if (NewResult)
			{
				NewResult->AttachToComponent(IngredientAnchor, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
				CurrentHeldItem = NewResult;
			}
		}
	}

	// Unlock player after cooking
	if (CurrentUser)
	{
		CurrentUser->UnlockFromStation();
		CurrentUser = nullptr;
	}
}
