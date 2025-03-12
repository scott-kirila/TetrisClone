// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "EnhancedInputSubsystems.h"

#include "TetrisPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class TETRISCLONE_API ATetrisPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UInputMappingContext* InputMappingContext;

	void BeginPlay() override;
};
