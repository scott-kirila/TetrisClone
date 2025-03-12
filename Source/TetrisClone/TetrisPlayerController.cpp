// Fill out your copyright notice in the Description page of Project Settings.


#include "TetrisPlayerController.h"

void ATetrisPlayerController::BeginPlay()
{
	Super::BeginPlay();

	auto Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());

	if (Subsystem)
	{
		Subsystem->AddMappingContext(InputMappingContext, 0);
	}
}
