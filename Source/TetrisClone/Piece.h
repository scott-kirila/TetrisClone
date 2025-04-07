// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"

#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"

#include "Piece.generated.h"

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterial* Color;

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UInputMappingContext* InputMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bCanMoveDown = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanRotate = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanSlide = true;

	TStaticArray<UStaticMeshComponent*, 4> Blocks;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanSpawn = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<APiece>> BlockTypes;
	
	FTimerHandle DropTimer;
	FTimerHandle StopLeftRightTimer;
	FTimerHandle SpawnTimer;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MoveHorizontally(const FInputActionValue& Value);
	void DownwardBurst();
	void Rotate();
	void OnDropTimeout();
	void OnSpawnTimeout();

	void Stop();
	void SpawnNewPiece();

	bool CanMoveToward(FVector Direction);
};
