// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Gameplay/Order/ActiveOrder.h"
#include "OrderWidget.generated.h"

class UWrapBox;
class UOrderItemWidget;
class UTextBlock;
class UProgressBar;

/**
 * UI Widget that represents a single active order (shown above a delivery station).
 * Displays required recipes, progress bar and timer.
 */
UCLASS()
class SUSHIRESTAURANT_API UOrderWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	/** Sets the current order to display and rebuilds the item widgets */
	UFUNCTION(BlueprintCallable)
	void SetOrderData(const FActiveOrder& InOrder);

	// Refresh only delivery counts
	UFUNCTION(BlueprintCallable)
	void RefreshDeliveryCounts(const FActiveOrder& InOrder);

protected:

	/** Container holding recipe item widgets (horizontal wrap) */
	UPROPERTY(meta = (BindWidget))
	UWrapBox* RecipeList_Box;

	/** Progress bar showing remaining time (0 to 1) */
	UPROPERTY(meta = (BindWidget))
	UProgressBar* Time_ProgressBar;

	/** Text showing countdown (e.g., "34s") */
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Timer_TextBlock;

	/** The class used to create each recipe item widget */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UOrderItemWidget> OrderItemWidgetClass;

	/** Instanced widgets currently visible in the list */
	UPROPERTY()
	TArray<UOrderItemWidget*> ActiveItems;

	/** Cached order data (copied from DeliveryStation) */
	FActiveOrder CachedOrder;

	/** Called every frame to update UI timer and delivery count */
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
};
