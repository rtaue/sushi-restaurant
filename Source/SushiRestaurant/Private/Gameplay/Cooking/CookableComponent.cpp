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

void UCookableComponent::StartCooking(float Duration)
{
	// Only allow the server to start cooking and only if the state is Raw
	if (GetOwnerRole() == ROLE_Authority && CookingState == ECookingState::Raw)
	{
		UE_LOG(LogCookableComponent, Log, TEXT("%s started cooking"), *GetOwner()->GetName());

		// Change state to Processing and start timer
		CookingState = ECookingState::Processing;

		GetWorld()->GetTimerManager().SetTimer(
			CookingTimer,
			this,
			&UCookableComponent::OnCookingComplete,
			Duration,
			false
		);
	}
}

void UCookableComponent::OnCookingComplete()
{
	// Server updates state to Cooked when timer finishes
	if (GetOwnerRole() == ROLE_Authority)
	{
		CookingState = ECookingState::Cooked;
		UE_LOG(LogCookableComponent, Log, TEXT("%s finished cooking"), *GetOwner()->GetName());
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

