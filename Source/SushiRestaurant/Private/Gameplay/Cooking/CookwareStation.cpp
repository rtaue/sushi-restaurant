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

	DOREPLIFETIME(ThisClass, CurrentHeldItem);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, CurrentUser, COND_None, REPNOTIFY_Always);
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

				Item->AttachToComponent(IngredientAnchor, FAttachmentTransformRules::SnapToTargetIncludingScale);
				Item->SetActorRelativeLocation(FVector::ZeroVector);
				Item->SetActorRelativeRotation(FRotator::ZeroRotator);
				
				CurrentHeldItem = Item;
				CurrentUser = Character;

				Character->LockToStation(this);
				Character->PlayInteractionMontage(InteractionMontage);

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

			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = GetInstigator();
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			if (AActor* Result = GetWorld()->SpawnActor<AActor>(Cookable->ResultActorClass, Loc, Rot, SpawnParams))
			{
				Result->AttachToComponent(IngredientAnchor, FAttachmentTransformRules::SnapToTargetIncludingScale);
				Result->SetActorRelativeLocation(FVector::ZeroVector);
				Result->SetActorRelativeRotation(FRotator::ZeroRotator);
				CurrentHeldItem = Result;
			}
		}
	}

	if (CurrentUser)
	{
		CurrentUser->StopInteractionMontage();
		CurrentUser->UnlockFromStation();
		CurrentUser = nullptr;
	}

	UE_LOG(LogCookwareStation, Log, TEXT("Finished cooking at station %s"), *GetName());
}

void ACookwareStation::OnRep_CurrentUser()
{
	if (CurrentUser && CurrentUser->IsLocallyControlled())
	{
		UpdateProgressWidget();
	}
	else
	{
		CookingProgressWidgetComponent->SetHiddenInGame(true);
	}
}

void ACookwareStation::InitProgressBarWidget()
{
	if (CookingProgressWidgetComponent && CookingProgressWidgetClass)
	{
		CookingProgressWidgetComponent->SetWidgetClass(CookingProgressWidgetClass);
		CookingProgressWidgetComponent->InitWidget();

		CookingProgressWidget = Cast<UCookingProgressWidget>(CookingProgressWidgetComponent->GetUserWidgetObject());
		if (CookingProgressWidget)
		{
			UpdateProgressWidget();
		}
		else
		{
			// Schedule delayed grab of CookingProgressWidget
			FTimerHandle TimerHandle;
			GetWorldTimerManager().SetTimer(TimerHandle, this, &ACookwareStation::SafeAssignCookingWidget, 0.1f, false);
		}
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

void ACookwareStation::SafeAssignCookingWidget()
{
	if (!CookingProgressWidget && CookingProgressWidgetComponent)
	{
		CookingProgressWidget = Cast<UCookingProgressWidget>(CookingProgressWidgetComponent->GetUserWidgetObject());

		if (CookingProgressWidget)
		{
			UE_LOG(LogCookwareStation, Warning, TEXT("CookingProgressWidget successfully assigned (delayed) on %s"), *GetName());
		}
		else
		{
			UE_LOG(LogCookwareStation, Error, TEXT("CookingProgressWidget is STILL null after delay on %s"), *GetName());
		}
	}
}
