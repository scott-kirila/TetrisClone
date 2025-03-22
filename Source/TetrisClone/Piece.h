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
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* Block0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USphereComponent* Collider0;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* Block1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USphereComponent* Collider1;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* Block2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USphereComponent* Collider2;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* Block3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USphereComponent* Collider3;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UInputAction* MoveHorizontallyAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UInputAction* DownwardBurstAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UInputAction* RotateAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UInputMappingContext* InputMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bCanMoveLeft = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bCanMoveRight = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bCanMoveDown = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanRotate = true;

	TStaticArray<UStaticMeshComponent*, 4> Blocks;
	
	FTimerHandle DropTimer;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MoveHorizontally(const FInputActionValue& Value);
	void DownwardBurst();
	void Rotate();
	void OnDropTimeout();

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);
};
