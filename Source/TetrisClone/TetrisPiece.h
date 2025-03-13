// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"

#include "InputActionValue.h"
#include "Engine/TimerHandle.h"

#include "TetrisPlayerController.h"

#include "TetrisPiece.generated.h"

enum StopSide
{
	Bottom,
	Left,
	Right,
	BothSides,
	None,
};

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
	TArray<TSubclassOf<ATetrisPiece>> BlockTypes;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ATetrisPiece> PieceToSpawn;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* RootBlock;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* Block1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* Block2;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* Block3;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ATetrisPlayerController* PlayerController;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UInputAction* LeftRightAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UInputAction* DownAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UInputAction* RotateAction;

	UPROPERTY(EditAnywhere)
	float TraceDistance = 10.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanMove = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanMoveLeft = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanMoveRight = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanRotate = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsStopping = false;
	
	FTimerHandle DropTimer;
	FTimerHandle SpawnTimer;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void LeftRight(const FInputActionValue& Value);
	void Down();
	void Rotate();
	void OnDropTimeout();
	void OnSpawnTimeout();

	StopSide CheckForObstacles(bool ForRotation);
	void CheckStoppingConditions();
	void Stop();
};
