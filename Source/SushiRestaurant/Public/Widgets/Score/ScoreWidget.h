// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ScoreWidget.generated.h"

class UTextBlock;
/**
 * Widget that displays the team score.
 */
UCLASS()
class SUSHIRESTAURANT_API UScoreWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	/// Called from GameState to update the score display
	UFUNCTION(BlueprintCallable)
	void UpdateScore(int32 NewScore) const;

protected:

	/// Text block that displays the score
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Score_TextBlock;
};
