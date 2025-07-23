// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Orders/OrderItemWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Gameplay/Recipe/RecipeData.h"

void UOrderItemWidget::Setup(URecipeData* InRecipe, const int32 Required, const int32 Delivered)
{
	Recipe = InRecipe;
	RequiredCount = Required;

	if (InRecipe && Icon_Image)
	{
		Icon_Image->SetBrushFromTexture(InRecipe->Icon);
	}

	UpdateDeliveredCount(Delivered);
}

void UOrderItemWidget::UpdateDeliveredCount(const int32 NewDelivered) const
{
	if (Count_TextBlock)
	{
		const FString Text = FString::Printf(TEXT("%d / %d"), NewDelivered, RequiredCount);
		Count_TextBlock->SetText(FText::FromString(Text));
	}
}
