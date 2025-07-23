// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Cooking/CookingProgressWidget.h"

#include "Components/ProgressBar.h"

void UCookingProgressWidget::SetProgress(const float InProgress)
{
	TargetProgress = FMath::Clamp(InProgress, 0.f, 1.f);
}

void UCookingProgressWidget::ResetProgress()
{
	TargetProgress = 0.f;
	CurrentProgress = 0.f;

	if (Cooking_ProgressBar)
	{
		Cooking_ProgressBar->SetPercent(0.f);
	}
}

void UCookingProgressWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// Smooth interpolation
	CurrentProgress = FMath::FInterpTo(CurrentProgress, TargetProgress, InDeltaTime, InterpSpeed);

	if (Cooking_ProgressBar)
	{
		Cooking_ProgressBar->SetPercent(CurrentProgress);
	}
}
