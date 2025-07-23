// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Cooking/DeliveryStation.h"

#include "Characters/SushiRestaurantCharacter.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "SushiRestaurant/SushiRestaurant.h"
#include "Utils/GameplayUtils.h"
#include "Widgets/Orders/OrderWidget.h"

ADeliveryStation::ADeliveryStation()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	// Create the widget component
	OrderWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("OrderWidget"));
	OrderWidgetComponent->SetupAttachment(RootComponent);
	OrderWidgetComponent->SetWidgetSpace(EWidgetSpace::World); // World space for 3D effect
	OrderWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 100.f));
	OrderWidgetComponent->SetDrawAtDesiredSize(true);
	OrderWidgetComponent->SetHiddenInGame(true); // Initially hidden
	OrderWidgetComponent->SetComponentTickEnabled(false); // Disable tick when hidden
}

void ADeliveryStation::BeginPlay()
{
	Super::BeginPlay();

	InitWidget();
	
	UpdateWidgetVisibility();
}

void ADeliveryStation::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Rotate the widget to face the player, only for local clients
	UpdateWidgetFacing();

	if (HasAuthority())
	{
		// Update order timer and check for failure
		UpdateOrderTimer();
	}
}

void ADeliveryStation::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, ActiveOrder);
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

void ADeliveryStation::Server_AssignOrder_Implementation(const FActiveOrder& NewOrder)
{
	ActiveOrder = NewOrder;

	// Server updates snapshot as well
	LastOrderSnapshot = ActiveOrder;

	UpdateOrderWidget();
	UpdateWidgetVisibility();

	UE_LOG(LogDeliveryStation, Log, TEXT("Assigned new order to station %s"), *GetName());
}

void ADeliveryStation::AssignOrder(const FActiveOrder& NewOrder)
{
	if (HasAuthority())
	{
		ActiveOrder = NewOrder;

		// Always update snapshot on the server
		LastOrderSnapshot = ActiveOrder;

		UpdateOrderWidget();
		UpdateWidgetVisibility();
	}
	else
	{
		Server_AssignOrder(NewOrder);
	}
}

void ADeliveryStation::OnRep_ActiveOrder()
{
	// Determine if the order is structurally new or just an update

	if (const bool bIsNewOrder = ActiveOrder.IsNewComparedTo(LastOrderSnapshot))
	{
		// New order assigned, rebuild full widget
		UpdateOrderWidget();
	}
	else
	{
		// Order already exists, only delivery counts updated
		if (OrderWidget)
		{
			OrderWidget->RefreshDeliveryCounts(ActiveOrder);
		}
	}

	// Update visibility either way
	UpdateWidgetVisibility();

	// Store this snapshot for future comparison
	LastOrderSnapshot = ActiveOrder;
}

bool ADeliveryStation::SubmitDish(AActor* DeliveredDish)
{
	if (!HasAuthority()) return false;
	
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

		if (OrderWidget)
		{
			OrderWidget->RefreshDeliveryCounts(ActiveOrder);
		}
		
		if (ActiveOrder.IsComplete())
		{
			GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Yellow,
			FString::Printf(TEXT("Order completed at %s!"), *GetName()));
			
			UE_LOG(LogDeliveryStation, Log, TEXT("Order completed at station %s!"), *GetName());
			
			OnOrderDelivered.Broadcast(true);

			UpdateWidgetVisibility();
			ClearOrderWithDelay();
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

void ADeliveryStation::ClearOrder()
{
	if (!HasAuthority())
	{
		// Only the server can clear the order
		return;
	}

	// Cancel pending clear, just in case
	GetWorld()->GetTimerManager().ClearTimer(ClearOrderHandle);

	// Reset data
	ActiveOrder.Clear();
	LastOrderSnapshot.Clear();

	// Update UI
	UpdateOrderWidget();
	UpdateWidgetVisibility();

	UE_LOG(LogDeliveryStation, Log, TEXT("Order cleared at station %s"), *GetName());

	// Broadcast
	OnOrderCleared.Broadcast();
}

void ADeliveryStation::ClearOrderWithDelay()
{
	// Delay before clearing
    GetWorld()->GetTimerManager().SetTimer(ClearOrderHandle, this, &ADeliveryStation::ClearOrder, 3.f, false);
}

void ADeliveryStation::InitWidget()
{
	if (OrderWidgetComponent && OrderWidgetClass)
	{
		OrderWidgetComponent->SetWidgetClass(OrderWidgetClass);
		OrderWidgetComponent->InitWidget();
		
		OrderWidget = Cast<UOrderWidget>(OrderWidgetComponent->GetUserWidgetObject());
	}
}

void ADeliveryStation::UpdateOrderTimer()
{
	// Update countdown only if order is still pending
	if (ActiveOrder.Status == EOrderStatus::Pending)
	{
		if (const float TimeRemaining = ActiveOrder.GetTimeRemaining(GetWorld()->GetTimeSeconds()); TimeRemaining <= 0.f)
		{
			ActiveOrder.Status = EOrderStatus::Failed;

			UpdateWidgetVisibility();
			
			OnOrderDelivered.Broadcast(false);
			ClearOrderWithDelay();

			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Red,
					FString::Printf(TEXT("Order FAILED at %s (timeout)"), *GetName()));
			}

			UE_LOG(LogDeliveryStation, Warning, TEXT("Order at %s FAILED due to timeout."), *GetName());
		}
	}
}

void ADeliveryStation::UpdateWidgetVisibility() const
{
	if (!OrderWidgetComponent) return;

	const bool bShouldBeVisible = ActiveOrder.Status == EOrderStatus::Pending;

	// Enable or disable visibility and ticking completely
	OrderWidgetComponent->SetHiddenInGame(!bShouldBeVisible);
	OrderWidgetComponent->SetComponentTickEnabled(bShouldBeVisible);
	OrderWidgetComponent->SetVisibility(bShouldBeVisible, true);

	UE_LOG(LogDeliveryStation, Log, TEXT("WidgetComponent is now %s"),
		bShouldBeVisible ? TEXT("VISIBLE") : TEXT("HIDDEN"));
}

void ADeliveryStation::UpdateWidgetFacing() const
{
	// Skip on dedicated server or non-local clients
	if (!IsNetMode(NM_DedicatedServer) && UGameplayUtils::IsLocallyControlled(GetWorld()))
	{
		if (OrderWidgetComponent)
		{
			// Get the local camera position
			if (const APlayerCameraManager* CamManager = UGameplayStatics::GetPlayerCameraManager(this, 0))
			{
				const FVector CamLoc = CamManager->GetCameraLocation();
				const FVector WidgetLoc = OrderWidgetComponent->GetComponentLocation();

				// Compute a rotation that faces the camera
				FRotator LookAtRotation = (CamLoc - WidgetLoc).Rotation();
				LookAtRotation.Pitch = 0.f;
				LookAtRotation.Roll = 0.f;

				OrderWidgetComponent->SetWorldRotation(LookAtRotation);
			}
		}
	}
}

void ADeliveryStation::UpdateOrderWidget() const
{
	if (OrderWidgetComponent && ActiveOrder.Status == EOrderStatus::Pending)
	{
		if (OrderWidget)
		{
			OrderWidget->SetOrderData(ActiveOrder);
			UpdateWidgetVisibility();
		}
	}
}

