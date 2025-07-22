// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/Cooking/StationBase.h"
#include "Gameplay/Order/ActiveOrder.h"
#include "DeliveryStation.generated.h"

struct FActiveOrder;

// Delegate to notify delivery result
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOrderDelivered, bool, bSuccess);

/**
 * 
 */
UCLASS()
class SUSHIRESTAURANT_API ADeliveryStation : public AStationBase
{
	GENERATED_BODY()

public:
	ADeliveryStation();

	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	// Interactable interface
	virtual void Interact_Implementation(AActor* Interactor) override;

	UFUNCTION(Server, Reliable)
	void Server_AssignOrder(const FActiveOrder& NewOrder);
	void AssignOrder(const FActiveOrder& NewOrder);

	UFUNCTION()
	void OnRep_ActiveOrder();

	UPROPERTY(ReplicatedUsing = OnRep_ActiveOrder, VisibleAnywhere, BlueprintReadOnly)
	FActiveOrder ActiveOrder;
	
	UFUNCTION(BlueprintCallable)
	bool SubmitDish(AActor* DeliveredDish);

	UPROPERTY(BlueprintAssignable)
	FOnOrderDelivered OnOrderDelivered;
	
};
