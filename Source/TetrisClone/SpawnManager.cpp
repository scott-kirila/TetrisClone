// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnManager.h"

#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"

#include "ScoreWidget.h"
#include "LineClearCameraShake.h"

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

	PlayerController = GetWorld()->GetFirstPlayerController();
	
	if (PlayerController)
	{
		auto Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());

		if (Subsystem)
		{
			Subsystem->AddMappingContext(InputMappingContext, 0);
		}
	}

	if (UIWidgetClass)
	{
		UIWidget = CreateWidget<UScoreWidget>(PlayerController, UIWidgetClass);
		UIWidget->AddToPlayerScreen();
	}
	
	TriggerSpawn();
}

void ASpawnManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UIWidget)
	{
		UIWidget->RemoveFromParent();
		UIWidget = nullptr;
	}
	
	Super::EndPlay(EndPlayReason);
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
	GetWorldTimerManager().ClearTimer(CurrentPiece->DropTimer);
	
	CheckRows();
	if (!ComponentsToDestroy.IsEmpty())
	{
		PlayerController->ClientStartCameraShake(ULineClearCameraShake::StaticClass());
		
		for (auto& Component : ComponentsToDestroy)
		{
			Component->DestroyComponent();
		}
	}

	GetWorldTimerManager().SetTimer(PostClearMoveTimer, this, &ASpawnManager::PostClearMoveCheck, 1.0f, false, 0.3f);
}

void ASpawnManager::TriggerSpawn()
{
	if (BlockTypes.IsEmpty()) return;

	if (!CurrentPiece)
	{
		auto Index = FMath::RandRange(0, BlockTypes.Num() - 1);
		auto SpawnMe = BlockTypes[Index];
		CurrentPiece = GetWorld()->SpawnActor<APiece>(SpawnMe, SpawnLocation, FRotator::ZeroRotator);
	} else
	{
		CurrentPiece = NextPiece;
		CurrentPiece->SetActorLocation(SpawnLocation);
		SpawnBlockCheck();
	}

	
	auto Index = FMath::RandRange(0, BlockTypes.Num() - 1);
	auto SpawnMe = BlockTypes[Index];
	
	auto PreviewLocation = FVector(-215, -25, 3525);
	
	
	NextPiece = GetWorld()->SpawnActor<APiece>(SpawnMe, PreviewLocation, FRotator::ZeroRotator);
	NextPiece->AddActorWorldOffset(NextPiece->PreviewOffset);
	
	float DropDelay = FMath::Pow(0.85f, Level);
	GetWorldTimerManager().SetTimer(CurrentPiece->DropTimer, CurrentPiece, &APiece::OnDropTimeout, DropDelay, true, DropDelay);
	
	PlayerController->Possess(CurrentPiece);
}

void ASpawnManager::SpawnBlockCheck()
{
	// Check for blocking piece
	bool Result = true;

	TArray<UStaticMeshComponent*> Meshes;
	CurrentPiece->GetComponents<UStaticMeshComponent>(Meshes);
	
	for (const auto& Mesh : Meshes)
	{
		FHitResult HitResult;
		FVector Start = Mesh->GetComponentLocation() + FVector::UpVector * 100.0f;
		FVector End = Start - FVector::UpVector * 100.0f;

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(CurrentPiece);

		auto bDidHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, QueryParams);
		auto Color = bDidHit ? FColor::Red : FColor::Green;
		
		DrawDebugLine(GetWorld(), Start, End, Color, false, 1, 0, 5);

		Result &= !bDidHit;
	}
	
	if (!Result)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::White, TEXT("Ding!"));
		UGameplayStatics::SetGamePaused(GetWorld(), true);
		UIWidget->RemoveFromParent();
		UIWidget = nullptr;
		DisplayGameOverScreen();
	}
}

void ASpawnManager::CheckRows()
{
	NumRowsToDelete = 0;
	ComponentsToDestroy.Empty();
	ComponentsToMove.Empty();
	
	for (float Height = 50.0f; Height < 2000.0f; Height += 100.0f)
	{
		FVector Start = FVector(-500.0f, -300.0f, Height);
		FVector End = FVector(600.0f, -300.0f, Height);

		TArray<FHitResult> HitResults;
		GetWorld()->LineTraceMultiByChannel(HitResults, Start, End, ECC_Camera);
		auto NumHits = FHitResult::GetNumOverlapHits(HitResults);
		
		if (NumHits == 10)
		{
			NumRowsToDelete++;
			
			for (auto& Hit : HitResults)
			{
				ComponentsToDestroy.Add(Hit.Component);
			}
		} else
		{
			for (auto& Hit : HitResults)
			{
				ComponentsToMove.Add(Hit.Component, NumRowsToDelete);
			}
		}
	}

	Score += NumRowsToDelete;
	if (NumRowsToDelete != 0)
	{
		UIWidget->UpdateScoreText(Score);
	}
	
	auto TempLevel = Score / 10;
	TempLevel++;
	if (TempLevel != Level)
	{
		Level = TempLevel;
		UIWidget->UpdateLevelText(Level);
	}
}

void ASpawnManager::PostClearMoveCheck()
{
	if (!ComponentsToMove.IsEmpty())
	{
		for (auto& Pair : ComponentsToMove)
		{
			auto CurrentLocation = Pair.Key->GetComponentLocation();
			auto NewLocation = FVector(CurrentLocation.X, CurrentLocation.Y, CurrentLocation.Z - 100.0f * Pair.Value);
			Pair.Key->SetWorldLocation(NewLocation);
		}
	}

	
	GetWorldTimerManager().SetTimer(SpawnTimer, this, &ASpawnManager::TriggerSpawn, 1.0f, false, 1.0f);
}
