// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Order/OrderManager.h"

#include "Gameplay/Cooking/DeliveryStation.h"
#include "Gameplay/Order/ActiveOrder.h"
#include "Kismet/GameplayStatics.h"
#include "SushiRestaurant/SushiRestaurant.h"

// Sets default values
AOrderManager::AOrderManager()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

void AOrderManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TimeUntilStationCheck -= DeltaTime;
	if (TimeUntilStationCheck <= 0.f)
	{
		AssignOrdersToStations();
		TimeUntilStationCheck = StationCheckInterval;
	}
}

void AOrderManager::BeginPlay()
{
	Super::BeginPlay();

	// Autodetect delivery stations if none assigned manually
	if (DeliveryStations.Num() == 0)
	{
		TArray<AActor*> Found;
		UGameplayStatics::GetAllActorsOfClass(this, ADeliveryStation::StaticClass(), Found);

		for (AActor* Actor : Found)
		{
			if (ADeliveryStation* Station = Cast<ADeliveryStation>(Actor))
			{
				DeliveryStations.Add(Station);
			}
		}

		UE_LOG(LogOrderManager, Log, TEXT("Found %d delivery stations."), DeliveryStations.Num());
	}

	AssignOrdersToStations();
	TimeUntilStationCheck = StationCheckInterval;
}

void AOrderManager::AssignOrdersToStations()
{
	UE_LOG(LogOrderManager, Log, TEXT("Checking for empty stations..."));

	for (ADeliveryStation* Station : DeliveryStations)
	{
		if (!Station) continue;

		if (Station->ActiveOrder.Status != EOrderStatus::Pending)
		{
			UE_LOG(LogOrderManager, Log, TEXT("Assigning new order to station %s"), *Station->GetName());

			TArray<FRecipeCount> RequiredRecipes;
			const int32 Num = FMath::RandRange(1, MaxNumOfOrderRecipes);

			// Keep track of how many of each recipe type we added
			TMap<URecipeData*, int32> TempRecipeCounts;

			for (int32 i = 0; i < Num; ++i)
			{
				URecipeData* R = AllRecipes[FMath::RandHelper(AllRecipes.Num())];
				TempRecipeCounts.FindOrAdd(R)++;
			}

			for (const TPair<URecipeData*, int32>& Pair : TempRecipeCounts)
			{
				RequiredRecipes.Add(FRecipeCount(Pair.Key, Pair.Value));
				UE_LOG(LogOrderManager, Log, TEXT(" -> %s x%d"), *GetNameSafe(Pair.Key), Pair.Value);
			}

			const FActiveOrder NewOrder(RequiredRecipes, 60.f);
			Station->AssignOrder(NewOrder);
			break;
		}
	}

	OnOrdersUpdated.Broadcast();
}

