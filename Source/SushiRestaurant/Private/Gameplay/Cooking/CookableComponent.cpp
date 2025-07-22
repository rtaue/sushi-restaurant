// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Cooking/CookableComponent.h"

#include "Net/UnrealNetwork.h"
#include "SushiRestaurant/SushiRestaurant.h"

// Sets default values for this component's properties
UCookableComponent::UCookableComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// Enable replication for this component
	SetIsReplicatedByDefault(true);

	// Initial cooking state is Raw
	CookingState = ECookingState::Raw;
}

void UCookableComponent::StartCooking(float Duration, AActor* StationActor)
{
	if (GetOwnerRole() == ROLE_Authority && CookingState == ECookingState::Raw && StationActor)
	{
		AActor* Owner = GetOwner();
		if (!Owner) return;

		CookingState = ECookingState::Processing;
		AttachedStation = StationActor;

		// Disable physics and collision
		Owner->SetActorEnableCollision(false);

		if (UPrimitiveComponent* Root = Cast<UPrimitiveComponent>(Owner->GetRootComponent()))
		{
			Root->SetSimulatePhysics(false);
		}

		// Attach to station for visual lock
		Owner->AttachToActor(StationActor, FAttachmentTransformRules::KeepWorldTransform);

		GetWorld()->GetTimerManager().SetTimer(
			CookingTimer,
			this,
			&UCookableComponent::OnCookingComplete,
			Duration,
			false
		);

		UE_LOG(LogCookableComponent, Log, TEXT("%s started cooking at station %s"), *Owner->GetName(), *StationActor->GetName());
	}
}

void UCookableComponent::OnCookingComplete()
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		AActor* Owner = GetOwner();
		if (!Owner) return;

		CookingState = ECookingState::Cooked;

		// // Detach and re-enable collision/physics
		// Owner->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		// Owner->SetActorEnableCollision(true);
		//
		// if (UPrimitiveComponent* Root = Cast<UPrimitiveComponent>(Owner->GetRootComponent()))
		// {
		// 	Root->SetSimulatePhysics(true);
		// }
		//
		// AttachedStation = nullptr;

		UE_LOG(LogCookableComponent, Log, TEXT("%s finished cooking"), *Owner->GetName());

		// Notify listeners
		OnCookingFinished.Broadcast(Owner);
	}
}

void UCookableComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	// Replicate CookingState to all clients
	DOREPLIFETIME(ThisClass, CookingState);
}

void UCookableComponent::OnRep_CookingState()
{
	// Log the new state on clients when replication updates
	UE_LOG(LogCookableComponent, Log, TEXT("%s state changed to %s"),
		*GetOwner()->GetName(),
		*UEnum::GetValueAsString(CookingState));
}

