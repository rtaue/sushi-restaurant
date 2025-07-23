// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "CookingProgressWidget.generated.h"

class UProgressBar;

/**
 * Simple widget to display a cooking progress bar with smooth updates.
 */
UCLASS()
class SUSHIRESTAURANT_API UCookingProgressWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	/** Set target progress value (0.0 - 1.0) */
	UFUNCTION(BlueprintCallable)
	void SetProgress(float InProgress);

	/** Optional reset if needed */
	UFUNCTION(BlueprintCallable)
	void ResetProgress();

protected:

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	/** Progress bar widget (bind via UMG) */
	UPROPERTY(meta = (BindWidget))
	UProgressBar* Cooking_ProgressBar;

	/** Target percent set externally */
	float TargetProgress = 0.f;

	/** Current smooth progress used for interpolation */
	float CurrentProgress = 0.f;

	/** Speed of interpolation */
	UPROPERTY(EditDefaultsOnly, Category = "Smooth")
	float InterpSpeed = 5.f;
};

