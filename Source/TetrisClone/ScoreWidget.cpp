// Fill out your copyright notice in the Description page of Project Settings.


#include "ScoreWidget.h"

void UScoreWidget::UpdateScoreText(int NewScore)
{
	auto NewText = FText::FromString(FString::Printf(TEXT("%d"), NewScore));
	ScoreText->SetText(NewText);
}

void UScoreWidget::UpdateLevelText(int NewLevel)
{
	auto NewText = FText::FromString(FString::Printf(TEXT("%d"), NewLevel));
	LevelText->SetText(NewText);
}
