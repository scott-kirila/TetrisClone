// Fill out your copyright notice in the Description page of Project Settings.


#include "Piece.h"

#include "EnhancedInputComponent.h"

// Sets default values
APiece::APiece()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	
	// Root Block
	Block0 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RootBlock"));
	Block0->SetupAttachment(RootComponent);
	Block0->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	Block0->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	// Block 1
	Block1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Block1"));
	Block1->SetupAttachment(RootComponent);
	Block1->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	Block1->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	
	// Block 2
	Block2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Block2"));
	Block2->SetupAttachment(RootComponent);
	Block2->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	Block2->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	// Block 3
	Block3 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Block3"));
	Block3->SetupAttachment(RootComponent);
	Block3->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	Block3->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	Blocks = { Block0, Block1, Block2, Block3 };
}

// Called when the game starts or when spawned
void APiece::BeginPlay()
{
	Super::BeginPlay();

	auto PlayerController = Cast<APlayerController>(Controller);

	if (PlayerController)
	{
		auto Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());

		if (Subsystem)
		{
			Subsystem->AddMappingContext(InputMappingContext, 0);
		}

		PlayerController->Possess(this);
	}

	GetWorldTimerManager().SetTimer(DropTimer, this, &APiece::OnDropTimeout, 1.0f, true, 1.0f);
	BlockedFromBelow.AddDynamic(this, &APiece::Stop);
}

// Called every frame
void APiece::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector Down = { 0.0f, 0.0f, -1.0f };
	if (!CanMoveToward(Down))
	{
		// Stop();
		SpawnNewPiece();
	}
}

// Called to bind functionality to input
void APiece::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	auto EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	if (EnhancedInputComponent)
	{
		EnhancedInputComponent->BindAction(MoveHorizontallyAction, ETriggerEvent::Triggered, this, &APiece::MoveHorizontally);
		EnhancedInputComponent->BindAction(DownwardBurstAction, ETriggerEvent::Triggered, this, &APiece::DownwardBurst);
		
		if (bCanRotate)
		{
			EnhancedInputComponent->BindAction(RotateAction, ETriggerEvent::Started, this, &APiece::Rotate);
		}
	}
}

void APiece::MoveHorizontally(const FInputActionValue& Value)
{
	if (bShouldStop) return;
	
	float ActionValue = Value.Get<float>();
	float Sign = FMath::Sign(ActionValue);

	FVector DirectionVector = { ActionValue, 0.0f, 0.0f };
	if (!CanMoveToward(DirectionVector)) return;
	
	auto CurrentLocation = GetActorLocation();
	SetActorLocation({CurrentLocation.X + Sign * 100.0f, CurrentLocation.Y, CurrentLocation.Z});
}

void APiece::DownwardBurst()
{
	FVector Direction = { 0.0f, 0.0f, -1.0f };
	if (!CanMoveToward(Direction)) return;
	
	auto CurrentLocation = GetActorLocation();
	SetActorLocation({CurrentLocation.X, CurrentLocation.Y, CurrentLocation.Z - 100.0f});
}

void APiece::Rotate()
{
	if (!bCanRotate) return;
	
	auto RootLocation = GetActorLocation();
	
	for (const auto Block : Blocks)
	{
		if (Block == Block0) continue;

		FVector Start = Block->GetComponentLocation();
		
		FVector ComponentVector = Start - RootLocation;
		ComponentVector = {ComponentVector.Z, ComponentVector.Y, -ComponentVector.X};
		FVector End = RootLocation + ComponentVector;

		auto XDiff = End.X - Start.X;
		auto ZDiff = End.Z - Start.Z;

		FVector Mid {};

		Mid = XDiff * ZDiff > 0.0f ? FVector(Start.X, Start.Y, End.Z) : FVector(End.X, Start.Y, Start.Z);
		
		FHitResult OutHit1;
		FHitResult OutHit2;

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);

		auto Color1 = FColor::Blue;
		auto Color2 = FColor::Orange;
		DrawDebugLine(GetWorld(), Start, Mid, Color1, false, 1, 0, 5);
		GetWorld()->LineTraceSingleByChannel(OutHit1, Start, Mid, ECC_Visibility, QueryParams);

		DrawDebugLine(GetWorld(), Mid, End, Color2, false, 1, 0, 5);
		GetWorld()->LineTraceSingleByChannel(OutHit2, Mid, End, ECC_Visibility, QueryParams);

		if (OutHit1.bBlockingHit || OutHit2.bBlockingHit) return;
	}
	
	auto Rotation = FRotator(-90.0f, 0.0f, 0.0f);
	AddActorLocalRotation(Rotation);
}

void APiece::OnDropTimeout()
{
	FVector Direction = { 0.0f, 0.0f, -1.0f};
	if (!CanMoveToward(Direction)) return;
	
	auto CurrentLocation = GetActorLocation();
	
	auto NewLocation = FVector(CurrentLocation.X, CurrentLocation.Y, CurrentLocation.Z - 100.0f);
	SetActorLocation(NewLocation);
}

void APiece::OnSpawnTimeout()
{
	if (BlockTypes.IsEmpty()) return;

	auto Index = FMath::RandRange(0, BlockTypes.Num() - 1);
	auto SpawnMe = BlockTypes[Index];
	
	GetWorld()->SpawnActor<APiece>(SpawnMe, FVector(0.0f, -300.0f, 950.0f), FRotator::ZeroRotator);
}

void APiece::Stop()
{
	if (!bShouldStop)
	{
		FScopeLock Lock(&Mutex);
		bShouldStop = true;

		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::White, TEXT("Triggered!"));
	}
	
	bCanRotate = false;

	if (GetWorldTimerManager().IsTimerActive(DropTimer))
	{
		GetWorldTimerManager().ClearTimer(DropTimer);
	}
}

void APiece::SpawnNewPiece()
{
	if (!bCanSpawn) return;
	
	bCanSpawn = false;
	GetWorldTimerManager().SetTimer(SpawnTimer, this, &APiece::OnSpawnTimeout, 1.0f, false, 1.0f);
}

bool APiece::CanMoveToward(FVector Direction)
{
	bool Result = true;

	TArray<UStaticMeshComponent*> Meshes;
	GetComponents<UStaticMeshComponent>(Meshes);
	
	for (const auto& Mesh : Meshes)
	{
		FHitResult HitResult;
		FVector Start = Mesh->GetComponentLocation();
		FVector End = Start + Direction * 100.0f;

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);

		auto bDidHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, QueryParams);
		auto Color = bDidHit ? FColor::Red : FColor::Green;
		DrawDebugLine(GetWorld(), Start, End, Color, false, 1, 0, 5);

		Result &= !bDidHit;
	}

	if (!Result && (Direction.Z < 0.0f))
	{
		BlockedFromBelow.Broadcast();
	}

	return Result;
}
