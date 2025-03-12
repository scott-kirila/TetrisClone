// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "TetrisPlayerController.h"

#include "InputActionValue.h"

#include "GameFramework/Pawn.h"
#include "TetrisPiece.generated.h"

UCLASS()
class TETRISCLONE_API ATetrisPiece : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ATetrisPiece();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ATetrisPlayerController* PlayerController;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UInputAction* RotateAction;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void Move(const FInputActionValue& Value);
	void Rotate(const FInputActionValue& Value);
};
