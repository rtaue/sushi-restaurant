// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/HoldableInterface.h"
#include "Interfaces/InteractableInterface.h"
#include "InteractableItem.generated.h"

UCLASS()
class SUSHIRESTAURANT_API AInteractableItem : public AActor, public IInteractableInterface, public IHoldableInterface
{
	GENERATED_BODY()

public:
	
	AInteractableItem();

	// Interact interface implementation
	virtual void Interact_Implementation(AActor* Interactor) override;

	// Holdable interface implementation
	virtual void OnPickedUp_Implementation() override;
	virtual void OnDropped_Implementation() override;

protected:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* Mesh;
};
