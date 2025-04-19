// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Piece.h"

#include "SpawnManager.generated.h"

UCLASS()
class TETRISCLONE_API ASpawnManager : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<APiece>> BlockTypes;

	FTimerHandle SpawnTimer;
	
	// Sets default values for this actor's properties
	ASpawnManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	APiece* CurrentPiece;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UInputMappingContext* InputMappingContext;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	APlayerController* PlayerController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector SpawnLocation = { 0.0f, -300.0f, 1950.0f };
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void StartSpawnTimer();
	void TriggerSpawn();

	void CheckRows();
};
