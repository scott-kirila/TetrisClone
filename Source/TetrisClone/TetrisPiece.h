// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"

#include "InputActionValue.h"
#include "Engine/TimerHandle.h"

#include "TetrisPlayerController.h"

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
	UInputAction* LeftRightAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UInputAction* DownAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UInputAction* RotateAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanMove = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanRotate = true;
	
	FTimerHandle DropTimer;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void OnComponentHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		FVector NormalImpulse, const FHitResult& Hit);

	void LeftRight(const FInputActionValue& Value);
	void Down();
	void Rotate();

	void OnDropTimeout();
};
