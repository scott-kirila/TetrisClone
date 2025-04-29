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

	GetWorldTimerManager().SetTimer(DropTimer, this, &APiece::OnDropTimeout, 1.0f, true, 1.0f);
	BlockedFromBelow.AddDynamic(this, &APiece::Stop);
}

// Called every frame
void APiece::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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
		EnhancedInputComponent->BindAction(DownwardBurstAction, ETriggerEvent::Started, this, &APiece::OnDownwardBurstStarted);
		EnhancedInputComponent->BindAction(DownwardBurstAction, ETriggerEvent::Canceled, this, &APiece::OnDownwardBurstCanceled);
		EnhancedInputComponent->BindAction(DownwardBurstAction, ETriggerEvent::Completed, this, &APiece::OnDownwardBurstCompleted);

		if (bRotatable)
		{
			EnhancedInputComponent->BindAction(RotateAction, ETriggerEvent::Started, this, &APiece::Rotate);
		}
	}
}

void APiece::MoveHorizontally(const FInputActionValue& Value)
{
	if ( bBlockedFromBelow && !bOneMoveAvailable ) return;
	
	float ActionValue = Value.Get<float>();

	// Perform direction check before allowing movement
	FVector DirectionVector = { ActionValue, 0.0f, 0.0f };
	if (!CanMoveToward(DirectionVector)) return;
	
	auto CurrentLocation = GetActorLocation();
	SetActorLocation({CurrentLocation.X + ActionValue * 100.0f, CurrentLocation.Y, CurrentLocation.Z});

	if (bBlockedFromBelow)
	{
		bOneMoveAvailable = false;
	}
}

void APiece::DownwardBurst()
{
	// Perform direction check before allowing movement
	FVector Direction = { 0.0f, 0.0f, -1.0f };
	if (!CanMoveToward(Direction)) return;

	auto CurrentLocation = GetActorLocation();
	SetActorLocation({CurrentLocation.X, CurrentLocation.Y, CurrentLocation.Z - 100.0f});
}

void APiece::OnDownwardBurstStarted()
{
	bDownwardBurstActive = true;
}

void APiece::OnDownwardBurstCanceled()
{
	bDownwardBurstActive = false;
}

void APiece::OnDownwardBurstCompleted()
{
	bDownwardBurstActive = false;
}

void APiece::Rotate()
{
	if (!bRotatable) return;
	
	if (CanRotate())
	{
		auto Rotation = FRotator(-90.0f, 0.0f, 0.0f);
		AddActorLocalRotation(Rotation);	
	}
}

void APiece::OnDropTimeout()
{
	if (bDownwardBurstActive) return;
	
	// Perform direction check before allowing movement
	FVector Direction = { 0.0f, 0.0f, -1.0f };
	if (!CanMoveToward(Direction))
	{
		return;
	}

	auto CurrentLocation = GetActorLocation();

	FVector NewLocation = { CurrentLocation.X, CurrentLocation.Y, CurrentLocation.Z - 100.0f };
	SetActorLocation(NewLocation);
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
	
	return Result;
}

bool APiece::CanRotate()
{
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

		if (OutHit1.bBlockingHit || OutHit2.bBlockingHit) return false;
	}

	return true;
}

void APiece::Stop()
{
	// if (!bShouldStop)
	// {
	// 	FScopeLock Lock(&Mutex);
	// 	bShouldStop = true;
	//
	// 	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::White, TEXT("Triggered!"));
	// }
	//
	// bRotatable = false;
	//
	// if (GetWorldTimerManager().IsTimerActive(DropTimer))
	// {
	// 	GetWorldTimerManager().ClearTimer(DropTimer);
	// }
}
