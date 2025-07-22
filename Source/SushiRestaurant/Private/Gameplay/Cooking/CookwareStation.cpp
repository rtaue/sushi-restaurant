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
				}
				
				Character->LockToStation(this);

				CurrentUser = Character;

				// Register unlock callback
				Cookable->OnCookingFinished.AddDynamic(this, &ACookwareStation::OnIngredientCooked);
				Cookable->StartCooking(CookingTime, this);
			}
		}
	}
}

void ACookwareStation::OnIngredientCooked(AActor* CookedActor)
{
	if (CurrentUser && CurrentUser->LockedStation == this)
	{
		CurrentUser->UnlockFromStation();
		CurrentUser = nullptr;
	}
}
