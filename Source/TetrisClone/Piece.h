// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"

#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "Components/StaticMeshComponent.h"

#include "Piece.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBlockedFromBelow);

UCLASS()
class TETRISCLONE_API APiece : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APiece();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* Root;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* Block0;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* Block1;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* Block2;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* Block3;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UInputAction* MoveHorizontallyAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UInputAction* DownwardBurstAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UInputAction* RotateAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanRotate = true;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bCanSpawn = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bShouldStop = false;

	TStaticArray<UStaticMeshComponent*, 4> Blocks;
	
	FTimerHandle DropTimer;

	FBlockedFromBelow BlockedFromBelow;
	mutable FCriticalSection Mutex;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MoveHorizontally(const FInputActionValue& Value);
	void DownwardBurst();
	void Rotate();
	void OnDropTimeout();

	bool CanMoveToward(FVector Direction);
	
	UFUNCTION()
	void Stop();
};
