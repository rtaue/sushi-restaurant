// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Orders/OrderWidget.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/WrapBox.h"
#include "Widgets/Orders/OrderItemWidget.h"

void UOrderWidget::SetOrderData(const FActiveOrder& InOrder)
{
	CachedOrder = InOrder;

	if (!RecipeList_Box || !OrderItemWidgetClass) return;

	// Clear existing items
	RecipeList_Box->ClearChildren();
	ActiveItems.Empty();

	// Create one widget per recipe in the order
	for (const FRecipeCount& RecipeCount : CachedOrder.RequiredRecipes)
	{
		if (UOrderItemWidget* Item = CreateWidget<UOrderItemWidget>(this, OrderItemWidgetClass))
		{
			Item->Setup(RecipeCount.Recipe, RecipeCount.Count, CachedOrder.GetDeliveredCountFor(RecipeCount.Recipe));
			RecipeList_Box->AddChildToWrapBox(Item);
			ActiveItems.Add(Item);
		}
	}
}

void UOrderWidget::RefreshDeliveryCounts(const FActiveOrder& InOrder)
{
	// Only update counts, not entire layout
	CachedOrder.RequiredRecipes = InOrder.RequiredRecipes;
	CachedOrder.DeliveredRecipes = InOrder.DeliveredRecipes;

	// Preserve timing info
	CachedOrder.StartTime = InOrder.StartTime;
	CachedOrder.TotalTime = InOrder.TotalTime;
	CachedOrder.Status = InOrder.Status;

	for (const UOrderItemWidget* Item : ActiveItems)
	{
		if (Item)
		{
			if (const URecipeData* Recipe = Item->GetRecipe())
			{
				const int32 Delivered = CachedOrder.GetDeliveredCountFor(Recipe);
				Item->UpdateDeliveredCount(Delivered);
			}
		}
	}
}

void UOrderWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (CachedOrder.Status != EOrderStatus::Pending) return;

	const float WorldTime = GetWorld()->GetTimeSeconds();
	const float TimeRemaining = CachedOrder.GetTimeRemaining(WorldTime);
	const float Percent = CachedOrder.TotalTime > 0.f ? TimeRemaining / CachedOrder.TotalTime : 0.f;

	// Update progress bar
	if (Time_ProgressBar)
	{
		const float SmoothedPercent = FMath::FInterpTo(Time_ProgressBar->GetPercent(), Percent, InDeltaTime, 5.f);
		Time_ProgressBar->SetPercent(SmoothedPercent);
	}

	// Update remaining seconds text
	if (Timer_TextBlock)
	{
		const int32 Seconds = FMath::CeilToInt(TimeRemaining);
		Timer_TextBlock->SetText(FText::FromString(FString::Printf(TEXT("%ds"), Seconds)));
	}
}
