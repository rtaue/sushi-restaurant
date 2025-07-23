// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/Cooking/StationBase.h"
#include "Gameplay/Order/ActiveOrder.h"
#include "Widgets/Orders/OrderWidget.h"
#include "DeliveryStation.generated.h"

class UWidgetComponent;
struct FActiveOrder;

// Delegate to notify when an order is delivered (true = success, false = fail)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOrderDelivered, bool, bSuccess);
// Delegate to notify when an order is cleared
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnOrderCleared);

/**
 * Delivery station where players can submit dishes based on active orders.
 * Each station can display a world-space UI and validate delivery.
 */
UCLASS()
class SUSHIRESTAURANT_API ADeliveryStation : public AStationBase
{
	GENERATED_BODY()

public:
	
	ADeliveryStation();

	// Actor lifecycle
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	// Interactable interface implementation
	virtual void Interact_Implementation(AActor* Interactor) override;

	/** Assign a new order from the server */
	UFUNCTION(Server, Reliable)
	void Server_AssignOrder(const FActiveOrder& NewOrder);

	/** Called on server or local authority to assign an order */
	void AssignOrder(const FActiveOrder& NewOrder);

	/** Called when ActiveOrder is updated via replication */
	UFUNCTION()
	void OnRep_ActiveOrder();

	/** Submit a dish from the player to attempt fulfilling the order */
	UFUNCTION(BlueprintCallable, Category = "Order")
	bool SubmitDish(AActor* DeliveredDish);

	/** Clear the current order and update UI (called by manager or timeout) */
	UFUNCTION(BlueprintCallable, Category = "Order")
	void ClearOrder();

	/** Calls ClearOrder() with delay */
	UFUNCTION(BlueprintCallable, Category = "Order")
	void ClearOrderWithDelay();

	/** Returns true if there's a pending order */
	UFUNCTION(BlueprintPure, Category = "Order")
	bool HasPendingOrder() const { return ActiveOrder.Status == EOrderStatus::Pending; }

protected:

	/** Inits order widget component */
	void InitWidget();

	/** Updates the order countdown and triggers failure if time expires */
	void UpdateOrderTimer();

	/** Updates the visibility of the widget based on ActiveOrder state */
	void UpdateWidgetVisibility() const;

	/** Makes the widget rotate to face the player's camera */
	void UpdateWidgetFacing() const;

	/** Updates the widget's data (only when ActiveOrder is valid) */
	void UpdateOrderWidget() const;

public:

	/** Widget component used to show the order in world space */
	UPROPERTY(VisibleAnywhere, Category = "UI")
	UWidgetComponent* OrderWidgetComponent;

	/** Widget class to use for the order UI */
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UOrderWidget> OrderWidgetClass;

	/** Order widget reference */
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	UOrderWidget* OrderWidget;

	/** The currently active order for this delivery station */
	UPROPERTY(ReplicatedUsing = OnRep_ActiveOrder, VisibleAnywhere, BlueprintReadOnly, Category = "Order")
	FActiveOrder ActiveOrder;

	/** Broadcasted when a delivery is accepted (success or failure) */
	UPROPERTY(BlueprintAssignable)
	FOnOrderDelivered OnOrderDelivered;
	
	/** Broadcasted when an order is cleared */
	UPROPERTY(BlueprintAssignable)
	FOnOrderCleared OnOrderCleared;

private:

	// Timer used to delay clearing the order
	FTimerHandle ClearOrderHandle;

	// Cached snapshot to compare replication updates
	FActiveOrder LastOrderSnapshot;
};

