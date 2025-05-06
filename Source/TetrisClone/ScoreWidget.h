// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "ScoreWidget.generated.h"

/**
 * 
 */
UCLASS()
class TETRISCLONE_API UScoreWidget : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UTextBlock> ScoreText;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UTextBlock> LevelText;

public:
	// UFUNCTION(BlueprintCallable)
	void UpdateScoreText(int NewScore);

	// UFUNCTION(BlueprintCallable)
	void UpdateLevelText(int NewLevel);
};
