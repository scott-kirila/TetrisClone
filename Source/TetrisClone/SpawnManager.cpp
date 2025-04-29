// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnManager.h"

#include "Engine/StaticMeshSocket.h"
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

	CheckSpawnConditions();
}

void ASpawnManager::CheckSpawnConditions()
{
	FVector Down = { 0.0f, 0.0f, -1.0f };
	if ( !CurrentPiece->CanMoveToward(Down) )
	{
		CurrentPiece->bBlockedFromBelow = true;
		
		if ( !GetWorldTimerManager().IsTimerActive(SlideAvailableTimer) )
		{
			GetWorldTimerManager().SetTimer(SlideAvailableTimer, this, &ASpawnManager::OnSlideAvailableTimeout, 1.0f, false, 0.5f);
		}
	} else
	{
		CurrentPiece->bBlockedFromBelow = false;
		CurrentPiece->bOneMoveAvailable = true;
	}
}

void ASpawnManager::OnSlideAvailableTimeout()
{
	FVector Down = { 0.0f, 0.0f, -1.0f };
	if ( !CurrentPiece->CanMoveToward(Down) && CurrentPiece->bBlockedFromBelow)
	{
		StartSpawnTimer();
	}
}

void ASpawnManager::StartSpawnTimer()
{
	if (CurrentPiece && !CurrentPiece->bCanSpawn) return;

	PlayerController->UnPossess();
	
	CurrentPiece->bCanSpawn = false;
	GetWorldTimerManager().SetTimer(SpawnTimer, this, &ASpawnManager::TriggerSpawn, 1.0f, false, 1.0f);

	// TArray<UStaticMeshComponent*> StaticMeshComponents;
	// CurrentPiece->GetComponents<UStaticMeshComponent>(StaticMeshComponents);
	//
	// for (auto& StaticMeshComponent : StaticMeshComponents)
	// {
	// 	StaticMeshComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Overlap);
	// }
	
	CheckRows();
}

void ASpawnManager::TriggerSpawn()
{
	if (BlockTypes.IsEmpty()) return;

	auto Index = FMath::RandRange(0, BlockTypes.Num() - 1);
	auto SpawnMe = BlockTypes[Index];
	
	CurrentPiece = GetWorld()->SpawnActor<APiece>(SpawnMe, SpawnLocation, FRotator::ZeroRotator);
	PlayerController->Possess(CurrentPiece);
}

void ASpawnManager::CheckRows()
{
	for (float Height = 50.0f; Height < 1000.0f; Height += 100.0f)
	{
		FVector Start = FVector(-500.0f, -300.0f, Height);
		FVector End = FVector(600.0f, -300.0f, Height);

		TArray<FHitResult> HitResults;
		bool bHit = GetWorld()->LineTraceMultiByChannel(HitResults, Start, End, ECC_Camera);
		auto NumHits = FHitResult::GetNumOverlapHits(HitResults);

		// GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::White, FString::Printf(TEXT("%d"), NumHits));
		
		if (NumHits == 10)
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::White, FString::Printf(TEXT("FULL ROW at Height = %f"), Height));
		}
	}
}
