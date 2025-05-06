// Fill out your copyright notice in the Description page of Project Settings.


#include "WidgetBackend.h"

void UWidgetBackend::UpdateScoreText(int NewScore)
{
	auto NewText = FText::FromString(FString::Printf(TEXT("%d"), NewScore));
	ScoreText->SetText(NewText);
}

void UWidgetBackend::UpdateLevelText(int NewLevel)
{
	auto NewText = FText::FromString(FString::Printf(TEXT("%d"), NewLevel));
	LevelText->SetText(NewText);
}
