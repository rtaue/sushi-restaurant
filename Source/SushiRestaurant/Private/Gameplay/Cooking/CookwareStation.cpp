// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Cooking/CookwareStation.h"

#include "Characters/SushiRestaurantCharacter.h"
#include "Components/WidgetComponent.h"
#include "Gameplay/Cooking/CookableComponent.h"
#include "Net/UnrealNetwork.h"
#include "SushiRestaurant/SushiRestaurant.h"
#include "Utils/GameplayUtils.h"
#include "Widgets/Cooking/CookingProgressWidget.h"

ACookwareStation::ACookwareStation()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	IngredientAnchor = CreateDefaultSubobject<USceneComponent>(TEXT("IngredientAnchor"));
	IngredientAnchor->SetupAttachment(RootComponent);

	CookingProgressWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("CookingProgressWidget"));
	CookingProgressWidgetComponent->SetupAttachment(RootComponent);
	CookingProgressWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
	CookingProgressWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 100.f));
	CookingProgressWidgetComponent->SetDrawSize(FVector2D(200.f, 20.f));
	CookingProgressWidgetComponent->SetHiddenInGame(true);
	CookingProgressWidgetComponent->SetComponentTickEnabled(false);
}

void ACookwareStation::BeginPlay()
{
	Super::BeginPlay();
	
	InitProgressBarWidget();
}

void ACookwareStation::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentUser && CurrentUser->IsLocallyControlled())
	{
		UpdateProgressWidget();
	}
	else
	{
		CookingProgressWidgetComponent->SetHiddenInGame(true);
	}
}

void ACookwareStation::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, CurrentUser);
}

void ACookwareStation::Interact_Implementation(AActor* Interactor)
{
	if (CurrentUser)
	{
		UE_LOG(LogCookwareStation, Warning, TEXT("Station %s is already in use."), *GetName());
		return;
	}

	if (ASushiRestaurantCharacter* Character = Cast<ASushiRestaurantCharacter>(Interactor))
	{
		if (Character->IsHoldingItem())
		{
			AActor* Item = Character->HeldItem;

			if (!AcceptedIngredientClass || !Item->IsA(AcceptedIngredientClass))
			{
				UE_LOG(LogCookwareStation, Warning, TEXT("Rejected item %s at %s"), *Item->GetName(), *GetName());
				return;
			}

			if (UCookableComponent* Cookable = Item->FindComponentByClass<UCookableComponent>())
			{
				Character->DropItem();

				if (UPrimitiveComponent* Root = Cast<UPrimitiveComponent>(Item->GetRootComponent()))
				{
					Root->SetSimulatePhysics(false);
					Item->SetActorEnableCollision(false);
				}

				Item->AttachToComponent(IngredientAnchor, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
				CurrentHeldItem = Item;
				CurrentUser = Character;

				Character->LockToStation(this);

				Cookable->OnCookingFinished.AddDynamic(this, &ACookwareStation::OnIngredientCooked);
				Cookable->StartCooking(CookingTime, this);

				UE_LOG(LogCookwareStation, Log, TEXT("Started cooking %s at station %s"), *Item->GetName(), *GetName());
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
		if (Cookable->ResultActorClass)
		{
			const FVector Loc = CookedActor->GetActorLocation();
			const FRotator Rot = CookedActor->GetActorRotation();

			CookedActor->Destroy();

			if (AActor* Result = GetWorld()->SpawnActor<AActor>(Cookable->ResultActorClass, Loc, Rot))
			{
				Result->AttachToComponent(IngredientAnchor, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
				CurrentHeldItem = Result;
			}
		}
	}

	if (CurrentUser)
	{
		CurrentUser->UnlockFromStation();
		CurrentUser = nullptr;
	}

	UE_LOG(LogCookwareStation, Log, TEXT("Finished cooking at station %s"), *GetName());
}

void ACookwareStation::InitProgressBarWidget()
{
	if (CookingProgressWidgetComponent && CookingProgressWidgetClass)
	{
		CookingProgressWidgetComponent->SetWidgetClass(CookingProgressWidgetClass);
		CookingProgressWidgetComponent->InitWidget();

		CookingProgressWidget = Cast<UCookingProgressWidget>(CookingProgressWidgetComponent->GetUserWidgetObject());
	}
}

void ACookwareStation::UpdateProgressWidget()
{
	bool bShouldShow = false;

	if (CurrentHeldItem)
	{
		if (const UCookableComponent* Cookable = CurrentHeldItem->FindComponentByClass<UCookableComponent>())
		{
			if (Cookable->IsCooking())
			{
				bShouldShow = true;

				// Update progress bar
				const float Percent = Cookable->GetElapsedTime(this) / CookingTime;
				if (CookingProgressWidget)
				{
					CookingProgressWidget->SetProgress(Percent);
				}
			}
		}
	}

	CookingProgressWidgetComponent->SetHiddenInGame(!bShouldShow);

	if (bShouldShow)
	{
		UGameplayUtils::UpdateWidgetFacing(CookingProgressWidgetComponent, this);
	}
}
