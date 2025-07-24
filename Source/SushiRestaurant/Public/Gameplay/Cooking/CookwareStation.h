// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/Cooking/StationBase.h"
#include "CookwareStation.generated.h"

class UWidgetComponent;
class UCookingProgressWidget;
class ASushiRestaurantCharacter;

/// Represents the current state of a cookware station
UENUM(BlueprintType)
enum class EStationState : uint8
{
	Idle        UMETA(DisplayName = "Idle"),
	Cooking     UMETA(DisplayName = "Cooking"),
	Finished    UMETA(DisplayName = "Finished")
};

/**
 * Cookware station that processes ingredients with a CookableComponent.
 */
UCLASS()
class SUSHIRESTAURANT_API ACookwareStation : public AStationBase
{
	GENERATED_BODY()

public:
	ACookwareStation();
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Interact interface implementation */
	virtual void Interact_Implementation(AActor* Interactor) override;

protected:

	/** Ingredient type accepted by this station */
	UPROPERTY(EditAnywhere, Category = "Cooking")
	TSubclassOf<AActor> AcceptedIngredientClass;

	/** Time required to cook the ingredient */
	UPROPERTY(EditAnywhere, Category = "Cooking")
	float CookingTime = 3.0f;

	/** Anchor where the item will be attached */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cooking")
	USceneComponent* IngredientAnchor;

	/** Montage to be played by the interacting character */
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* InteractionMontage;

	/** Widget component to show cooking progress */
	UPROPERTY(VisibleAnywhere, Category = "UI")
	UWidgetComponent* CookingProgressWidgetComponent;

	/** Widget class to instantiate */
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UCookingProgressWidget> CookingProgressWidgetClass;

	/** Runtime reference to the UI widget */
	UPROPERTY()
	UCookingProgressWidget* CookingProgressWidget;

	/** Current ingredient being cooked */
	UPROPERTY(Replicated)
	AActor* CurrentHeldItem;

	/** Player currently using the station */
	UPROPERTY(ReplicatedUsing = OnRep_CurrentUser)
	ASushiRestaurantCharacter* CurrentUser;
	
	UFUNCTION()
	void OnRep_CurrentUser();

private:

	/** Initialize progress bar widget */
	void InitProgressBarWidget();

	/** Update progress bar visibility & facing */
	void UpdateProgressWidget();

	void SafeAssignCookingWidget();

	/** Handle when cooking is complete */
	UFUNCTION()
	void OnIngredientCooked(AActor* CookedActor);
	
};

