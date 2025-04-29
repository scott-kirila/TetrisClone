// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnManager.h"

#include "Kismet/GameplayStatics.h"

// Sets default values
ASpawnManager::ASpawnManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASpawnManager::BeginPlay()
{
	Super::BeginPlay();

	PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	if (PlayerController)
	{
		auto Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());

		if (Subsystem)
		{
			Subsystem->AddMappingContext(InputMappingContext, 0);
		}
	}
	
	TriggerSpawn();
}

// Called every frame
void ASpawnManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!CurrentPiece) return;

	FVector Down = { 0.0f, 0.0f, -1.0f };
	if ( !CurrentPiece->CanMoveToward(Down) )
	{
		CurrentPiece->bBlockedFromBelow = true;
		
		if ( !GetWorldTimerManager().IsTimerActive(WaitTimer) )
		{
			GetWorldTimerManager().SetTimer(WaitTimer, this, &ASpawnManager::OnWaitTimeout, 1.0f, false, 0.5f);
		}
	} else
	{
		CurrentPiece->bBlockedFromBelow = false;
		CurrentPiece->bOneMoveAvailable = true;
	}
}

void ASpawnManager::StartSpawnTimer()
{
	if (CurrentPiece && !CurrentPiece->bCanSpawn) return;

	PlayerController->UnPossess();
	
	CurrentPiece->bCanSpawn = false;
	GetWorldTimerManager().SetTimer(SpawnTimer, this, &ASpawnManager::TriggerSpawn, 1.0f, false, 1.0f);
}

void ASpawnManager::TriggerSpawn()
{
	if (BlockTypes.IsEmpty()) return;

	auto Index = FMath::RandRange(0, BlockTypes.Num() - 1);
	auto SpawnMe = BlockTypes[Index];
	
	CurrentPiece = GetWorld()->SpawnActor<APiece>(SpawnMe, SpawnLocation, FRotator::ZeroRotator);
	PlayerController->Possess(CurrentPiece);
}

void ASpawnManager::OnWaitTimeout()
{
	FVector Down = { 0.0f, 0.0f, -1.0f };
	if ( !CurrentPiece->CanMoveToward(Down) && CurrentPiece->bBlockedFromBelow)
	{
		StartSpawnTimer();
	}
}

void ASpawnManager::CheckRows()
{
	
}
