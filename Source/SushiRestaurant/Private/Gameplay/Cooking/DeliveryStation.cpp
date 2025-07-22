// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Cooking/DeliveryStation.h"

#include "Characters/SushiRestaurantCharacter.h"
#include "Net/UnrealNetwork.h"
#include "SushiRestaurant/SushiRestaurant.h"

ADeliveryStation::ADeliveryStation()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

void ADeliveryStation::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ActiveOrder.Status == EOrderStatus::Pending)
	{
		ActiveOrder.TimeRemaining -= DeltaTime;

		if (ActiveOrder.TimeRemaining <= 0.f)
		{
			ActiveOrder.Status = EOrderStatus::Failed;

			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Red,
					FString::Printf(TEXT("Order FAILED at %s (timeout)"), *GetName()));
			}
			
			UE_LOG(LogDeliveryStation, Warning, TEXT("Order at %s FAILED due to timeout."), *GetName());
			OnOrderDelivered.Broadcast(false);
		}
	}
}

void ADeliveryStation::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, ActiveOrder);
}

void ADeliveryStation::Server_AssignOrder_Implementation(const FActiveOrder& NewOrder)
{
	ActiveOrder = NewOrder;
	OnRep_ActiveOrder();
	
	UE_LOG(LogDeliveryStation, Log, TEXT("Assigned new order to station %s"), *GetName());
}

void ADeliveryStation::AssignOrder(const FActiveOrder& NewOrder)
{
	if (HasAuthority())
	{
		ActiveOrder = NewOrder;
		OnRep_ActiveOrder();
	}
	else
	{
		Server_AssignOrder(NewOrder);
	}
}

void ADeliveryStation::Interact_Implementation(AActor* Interactor)
{
	if (!Interactor) return;

	ASushiRestaurantCharacter* Character = Cast<ASushiRestaurantCharacter>(Interactor);
	if (!Character) return;

	// Tries to deliver dish
	if (AActor* HeldItem = Character->HeldItem)
	{
		if (SubmitDish(HeldItem))
		{
			// Dish accepted, maybe destroy or give feedback
			UE_LOG(LogDeliveryStation, Log, TEXT("Delivered dish %s to %s"), *HeldItem->GetName(), *GetName());
			Character->DropItem();
			HeldItem->Destroy();
		}
		else
		{
			// Reject -> maybe visual or sound feedback
			UE_LOG(LogDeliveryStation, Warning, TEXT("Dish rejected at %s"), *GetName());
		}
	}
}

void ADeliveryStation::OnRep_ActiveOrder()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Cyan,
			FString::Printf(TEXT("Replicated order updated at %s"), *GetName()));
	}
}

bool ADeliveryStation::SubmitDish(AActor* DeliveredDish)
{
	if (ActiveOrder.Status != EOrderStatus::Pending)
	{
		UE_LOG(LogDeliveryStation, Warning, TEXT("Station %s not accepting dishes (Status: %d)"), *GetName(), (int32)ActiveOrder.Status);

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Red, FString::Printf(TEXT("Station %s is not accepting deliveries."), *GetName()));
		}
		return false;
	}

	if (ActiveOrder.TryDeliverDish(DeliveredDish))
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Green,
				FString::Printf(TEXT("Dish %s accepted at %s"), *DeliveredDish->GetName(), *GetName()));
		}
		
		if (ActiveOrder.IsComplete())
		{
			GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Yellow,
			FString::Printf(TEXT("Order completed at %s!"), *GetName()));
			
			UE_LOG(LogDeliveryStation, Log, TEXT("Order completed at station %s!"), *GetName());
			OnOrderDelivered.Broadcast(true);
		}
		return true;
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Red,
			FString::Printf(TEXT("Dish %s is not valid for this order!"), *DeliveredDish->GetName()));
	}
	
	UE_LOG(LogDeliveryStation, Warning, TEXT("Dish %s is not valid for order at %s"), *DeliveredDish->GetName(), *GetName());
	return false;
}


