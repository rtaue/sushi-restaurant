// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OrderItemWidget.generated.h"

class UTextBlock;
class UImage;
class URecipeData;

/**
 * Widget representing a single recipe entry in the order
 */
UCLASS()
class SUSHIRESTAURANT_API UOrderItemWidget : public UUserWidget
{
	GENERATED_BODY()
    
public:

	/** Initializes the widget with recipe and counts */
	UFUNCTION(BlueprintCallable)
	void Setup(URecipeData* InRecipe, int32 Required, int32 Delivered);

	/** Updates only the delivery count */
	UFUNCTION(BlueprintCallable)
	void UpdateDeliveredCount(int32 NewDelivered) const;

	/** Get order recipe data */
	UFUNCTION(BlueprintCallable)
	URecipeData* GetRecipe() const { return Recipe; }

protected:

	/** The recipe associated with this item */
	UPROPERTY(BlueprintReadOnly)
	URecipeData* Recipe;

	UPROPERTY(meta = (BindWidget))
	UImage* Icon_Image;
    
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Count_TextBlock;

	int32 RequiredCount = 0;
};
