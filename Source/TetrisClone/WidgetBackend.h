// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "WidgetBackend.generated.h"

/**
 * 
 */
UCLASS()
class TETRISCLONE_API UWidgetBackend : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* ScoreText;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* LevelText;

public:
	// UFUNCTION(BlueprintCallable)
	void UpdateScoreText(int NewScore);

	// UFUNCTION(BlueprintCallable)
	void UpdateLevelText(int NewLevel);
};
