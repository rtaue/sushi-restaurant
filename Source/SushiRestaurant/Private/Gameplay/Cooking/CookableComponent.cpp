// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Cooking/CookableComponent.h"

#include "Net/UnrealNetwork.h"
#include "SushiRestaurant/SushiRestaurant.h"

// Sets default values for this component's properties
UCookableComponent::UCookableComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
	
	CookingState = ECookingState::Raw;
}

void UCookableComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, CookingState);
}

void UCookableComponent::StartCooking(const float Duration, AActor* InStation)
{
	if (GetOwnerRole() != ROLE_Authority || CookingState != ECookingState::Raw || !InStation)
	{
		UE_LOG(LogCookableComponent, Warning, TEXT("StartCooking failed on %s"), *GetOwner()->GetName());
		return;
	}

	AActor* Owner = GetOwner();
	CookingState = ECookingState::Processing;
	AttachedStation = InStation;
	CookingDuration = Duration;
	CookingStartTime = GetWorld()->GetTimeSeconds();

	// Attach visually
	Owner->SetActorEnableCollision(false);

	if (UPrimitiveComponent* Root = Cast<UPrimitiveComponent>(Owner->GetRootComponent()))
	{
		Root->SetSimulatePhysics(false);
	}

	Owner->AttachToActor(InStation, FAttachmentTransformRules::KeepWorldTransform);

	// Server-side timer
	GetWorld()->GetTimerManager().SetTimer(
		CookingTimer,
		this,
		&UCookableComponent::OnCookingComplete,
		Duration,
		false
	);

	UE_LOG(LogCookableComponent, Log,
		TEXT("%s started cooking at station %s for %.2fs"),
		*Owner->GetName(),
		*InStation->GetName(),
		Duration);
}

float UCookableComponent::GetElapsedTime(const UObject* WorldContext) const
{
	if (CookingState != ECookingState::Processing || !WorldContext) return 0.f;

	const float WorldTime = WorldContext->GetWorld()->GetTimeSeconds();
	return FMath::Clamp(WorldTime - CookingStartTime, 0.f, CookingDuration);
}

float UCookableComponent::GetRemainingTime(const UObject* WorldContext) const
{
	if (CookingState != ECookingState::Processing || !WorldContext) return 0.f;

	const float WorldTime = WorldContext->GetWorld()->GetTimeSeconds();
	return FMath::Clamp((CookingStartTime + CookingDuration) - WorldTime, 0.f, CookingDuration);
}

void UCookableComponent::OnRep_CookingState() const
{
	UE_LOG(LogCookableComponent, Log,
		TEXT("%s replicated new state: %s"),
		*GetOwner()->GetName(),
		*UEnum::GetValueAsString(CookingState));
}

void UCookableComponent::OnCookingComplete()
{
	if (GetOwnerRole() != ROLE_Authority) return;

	CookingState = ECookingState::Cooked;
	OnCookingFinished.Broadcast(GetOwner());

	UE_LOG(LogCookableComponent, Log,
		TEXT("%s finished cooking (duration: %.2fs)"),
		*GetOwner()->GetName(),
		CookingDuration);
}

