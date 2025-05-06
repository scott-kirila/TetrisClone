// Fill out your copyright notice in the Description page of Project Settings.

/*
 * Stopping condition: a downward trace from at least one block hits something.
 *
 * However, we want to allow a short window in which the player can move the
 * piece AT MOST one space. 
 *
 * We could try handling this in the Drop Timer, perhaps checking
 * if there is a downward hit AND if a slide is still available. If not,
 * we unpossess the piece and spawn a new one.
*/

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"

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
	TObjectPtr<USceneComponent> Root;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> Block0;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> Block1;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> Block2;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> Block3;

	TStaticArray<TWeakObjectPtr<UStaticMeshComponent>, 4> Blocks;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UInputAction> MoveHorizontallyAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UInputAction> DownwardBurstAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UInputAction> RotateAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bRotatable = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bDownwardBurstActive = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bBlockedFromBelow = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bOneMoveAvailable = true;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bCanSpawn = true;
	
	FTimerHandle DropTimer;

	FBlockedFromBelow BlockedFromBelow;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MoveHorizontally(const FInputActionValue& Value);
	void DownwardBurst();
	void OnDownwardBurstStarted();
	void OnDownwardBurstCanceled();
	void OnDownwardBurstCompleted();
	void Rotate();
	void OnDropTimeout();

	bool CanMoveToward(const FVector& Direction) const;
	bool CanRotate();
};
