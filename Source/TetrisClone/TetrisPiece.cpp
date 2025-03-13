// Fill out your copyright notice in the Description page of Project Settings.


#include "TetrisPiece.h"

#include "EnhancedInputComponent.h"

// Sets default values
ATetrisPiece::ATetrisPiece()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootBlock = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RootBlock"));
    SetRootComponent(RootBlock);

	Block1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Block1"));
	Block1->SetupAttachment(RootComponent);

	Block2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Block2"));
	Block2->SetupAttachment(RootComponent);

	Block3 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Block3"));
	Block3->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ATetrisPiece::BeginPlay()
{
	Super::BeginPlay();
	
	PlayerController = Cast<ATetrisPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorld()));

	if (PlayerController)
	{
		PlayerController->Possess(this);
		
	}

	GetWorldTimerManager().SetTimer(DropTimer, this, &ATetrisPiece::OnDropTimeout, 1.0f, true, 1.0f);
}

// Called every frame
void ATetrisPiece::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ATetrisPiece::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	auto EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	if (EnhancedInputComponent)
	{
		EnhancedInputComponent->BindAction(LeftRightAction, ETriggerEvent::Triggered, this, &ATetrisPiece::LeftRight);
		EnhancedInputComponent->BindAction(DownAction, ETriggerEvent::Triggered, this, &ATetrisPiece::Down);
		
		if (bCanRotate)
		{
			EnhancedInputComponent->BindAction(RotateAction, ETriggerEvent::Started, this, &ATetrisPiece::Rotate);
		}
	}
}

void ATetrisPiece::LeftRight(const FInputActionValue& Value)
{
	if (!bCanMove) return;

	float ActionValue = Value.Get<float>();
	float Direction = FMath::Sign(ActionValue);
	
	auto CurrentLocation = GetActorLocation();

	if ((Direction > 0.0f && bCanMoveRight) || (Direction < 0.0f && bCanMoveLeft))
	{
		SetActorLocation({CurrentLocation.X + Direction * 100.0f, CurrentLocation.Y, CurrentLocation.Z});
	}

	CheckStoppingConditions();
}

void ATetrisPiece::Down()
{
	if (!bCanMove) return;

	auto CurrentLocation = GetActorLocation();
	SetActorLocation({CurrentLocation.X, CurrentLocation.Y, CurrentLocation.Z - 100.0f});

	CheckStoppingConditions();
}

void ATetrisPiece::Rotate()
{
	if (!bCanMove || !bCanRotate) return;

	auto Rotation = FRotator(-90.0f, 0.0f, 0.0f);
	AddActorLocalRotation(Rotation);

	CheckStoppingConditions();
}

void ATetrisPiece::OnDropTimeout()
{
	// if (!bCanMove) return;
	
	auto CurrentLocation = GetActorLocation();
	
	auto NewLocation = FVector(CurrentLocation.X, CurrentLocation.Y, CurrentLocation.Z - 100.0f);
	SetActorLocation(NewLocation);

	CheckStoppingConditions();
}

void ATetrisPiece::OnSpawnTimeout()
{
	if (BlockTypes.IsEmpty()) return;

	auto Index = FMath::RandRange(0, BlockTypes.Num() - 1);
	auto SpawnMe = BlockTypes[Index];
	
	GetWorld()->SpawnActor<ATetrisPiece>(SpawnMe, FVector(-20.0f, -210.0f, 650.0f), FRotator::ZeroRotator);
}

void ATetrisPiece::CheckStoppingConditions()
{
	bCanMoveLeft = true;
	bCanMoveRight = true;
	
	TArray<USceneComponent*> Meshes;
	GetComponents(Meshes, true);

	FVector Right = FVector::ForwardVector;
	FVector Down = -FVector::UpVector;
	FVector Left = -Right;
	TArray<FVector> Directions { Right, Down, Left };
	
	for (auto& Mesh : Meshes)
	{
		for (const auto& Direction : Directions)
		{
			FVector Start = Mesh->GetComponentLocation() + Direction * 45.0f;
			FVector End = Start + Direction * TraceDistance;
			FHitResult OutHit;
			DrawDebugLine(GetWorld(), Start, End, FColor::Green, true, 1, 0, 5);
			GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_WorldStatic);

			if (!bIsStopping && OutHit.bBlockingHit && OutHit.Component->GetOwner() != this)
			{
				if (Direction.Z == -1.0)
				{
					bIsStopping = true;
					Stop();
					return;
				}

				if (Direction.X == 1.0f)
				{
					bCanMoveRight &= false;
				} else if (Direction.X == -1.0f)
				{
					bCanMoveLeft &= false;
				}
			}
		}
	}
}

void ATetrisPiece::Stop()
{
	bCanMove = false;
	bCanMoveLeft = false;
	bCanMoveRight = false;
	bCanRotate = false;
	GetWorldTimerManager().ClearTimer(DropTimer);
	GetWorldTimerManager().SetTimer(SpawnTimer, this, &ATetrisPiece::OnSpawnTimeout, 1.0f, false, 1.0f);
}
