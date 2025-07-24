// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Score/ScoreWidget.h"

#include "Components/TextBlock.h"

void UScoreWidget::UpdateScore(const int32 NewScore) const
{
	if (Score_TextBlock)
	{
		Score_TextBlock->SetText(FText::AsNumber(NewScore));
	}
}
