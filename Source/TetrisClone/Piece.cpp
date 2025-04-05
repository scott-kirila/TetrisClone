// Fill out your copyright notice in the Description page of Project Settings.


#include "Piece.h"

#include "EnhancedInputComponent.h"

#include "Block.h"

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

	Collider0 = CreateDefaultSubobject<USphereComponent>(TEXT("RootColliderA"));
	Collider0->SetupAttachment(Block0);
	Collider0->SetSphereRadius(70.0f);

	// Block 1
	Block1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Block1"));
	Block1->SetupAttachment(RootComponent);

	Collider1 = CreateDefaultSubobject<USphereComponent>(TEXT("Collider1A"));
	Collider1->SetupAttachment(Block1);
	Collider1->SetSphereRadius(70.0f);
	
	// Block 2
	Block2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Block2"));
	Block2->SetupAttachment(RootComponent);

	Collider2 = CreateDefaultSubobject<USphereComponent>(TEXT("Collider2A"));
	Collider2->SetupAttachment(Block2);
	Collider2->SetSphereRadius(70.0f);

	// Block 3
	Block3 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Block3"));
	Block3->SetupAttachment(RootComponent);

	Collider3 = CreateDefaultSubobject<USphereComponent>(TEXT("Collider3A"));
	Collider3->SetupAttachment(Block3);
	Collider3->SetSphereRadius(70.0f);

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

	Collider0->OnComponentBeginOverlap.AddDynamic(this, &APiece::OnBeginOverlap);
	Collider0->OnComponentEndOverlap.AddDynamic(this, &APiece::OnEndOverlap);

	Collider1->OnComponentBeginOverlap.AddDynamic(this, &APiece::OnBeginOverlap);
	Collider1->OnComponentEndOverlap.AddDynamic(this, &APiece::OnEndOverlap);

	Collider2->OnComponentBeginOverlap.AddDynamic(this, &APiece::OnBeginOverlap);
	Collider2->OnComponentEndOverlap.AddDynamic(this, &APiece::OnEndOverlap);

	Collider3->OnComponentBeginOverlap.AddDynamic(this, &APiece::OnBeginOverlap);
	Collider3->OnComponentEndOverlap.AddDynamic(this, &APiece::OnEndOverlap);

	GetWorldTimerManager().SetTimer(DropTimer, this, &APiece::OnDropTimeout, 1.0f, true, 1.0f);

}

// Called every frame
void APiece::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (!bCanMoveDown)
	{
		Stop();
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
	float ActionValue = Value.Get<float>();
	float Direction = FMath::Sign(ActionValue);
	
	auto CurrentLocation = GetActorLocation();

	if ((bCanMoveLeft && Direction < 0.0f) || (bCanMoveRight && Direction > 0.0f))
	{
		SetActorLocation({CurrentLocation.X + Direction * 100.0f, CurrentLocation.Y, CurrentLocation.Z});
	}
}

void APiece::DownwardBurst()
{
	if (!bCanMoveDown) return;
	
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

		auto Color1 = FColor::Blue;
		auto Color2 = FColor::Orange;
		DrawDebugLine(GetWorld(), Start, Mid, Color1, false, 2, 0, 5);
		GetWorld()->LineTraceSingleByChannel(OutHit1, Start, Mid, ECC_WorldStatic);

		DrawDebugLine(GetWorld(), Mid, End, Color2, false, 2, 0, 5);
		GetWorld()->LineTraceSingleByChannel(OutHit2, Mid, End, ECC_WorldStatic);

		if (OutHit1.bBlockingHit || OutHit2.bBlockingHit) return;
	}
	
	auto Rotation = FRotator(-90.0f, 0.0f, 0.0f);
	AddActorLocalRotation(Rotation);
}

void APiece::OnDropTimeout()
{
	if (!bCanMoveDown) return;
	
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
	bCanMoveLeft  = false;
	bCanMoveRight = false;
	bCanRotate    = false;

	Collider0->SetCollisionResponseToAllChannels(ECR_Block);
	Collider1->SetCollisionResponseToAllChannels(ECR_Block);
	Collider2->SetCollisionResponseToAllChannels(ECR_Block);
	Collider3->SetCollisionResponseToAllChannels(ECR_Block);
	
	GetWorldTimerManager().ClearTimer(DropTimer);
}

void APiece::SpawnNewPiece()
{
	if (!bCanSpawn) return;
	
	bCanSpawn = false;
	GetWorldTimerManager().SetTimer(SpawnTimer, this, &APiece::OnSpawnTimeout, 1.0f, false, 1.0f);
}

void APiece::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == this) return;

	FVector Down = -FVector::UpVector;
	FVector Left = -FVector::ForwardVector;
	FVector Right = -Left;
	TArray<FVector> Directions = { Down, Left, Right };

	for (const auto& Direction : Directions)
	{
		FHitResult HitResult;
		FVector Start = OverlappedComponent->GetComponentLocation();
		FVector End = Start + Direction * 100.0f;

		FCollisionQueryParams QueryParameters;
		QueryParameters.AddIgnoredActor(this);

		DrawDebugLine(GetWorld(), Start, End, FColor::Red, true, 1, 0, 5);
		auto bDidHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, QueryParameters);
		
		bCanMoveDown  &= (Direction != Down)  || !bDidHit;
		bCanMoveLeft  &= (Direction != Left)  || !bDidHit;
		bCanMoveRight &= (Direction != Right) || !bDidHit;
	}
}

void APiece::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (OtherActor == this) return;

	// Changing the actual values early could introduce movement bugs!
	bool _bCanMoveLeft  = true;
	bool _bCanMoveRight = true;

	FVector Left = -FVector::ForwardVector;
	FVector Right = -Left;
	TArray<FVector> Directions = { Left, Right };
	
	for (const auto& Direction : Directions)
	{
		FHitResult HitResult;
		FVector Start = OverlappedComponent->GetComponentLocation();
		FVector End = Start + Direction * 100.0f;

		FCollisionQueryParams QueryParameters;
		QueryParameters.AddIgnoredActor(this);
		
		DrawDebugLine(GetWorld(), Start, End, FColor::Green, true, 1, 0, 5);
		auto bDidHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, QueryParameters);

		_bCanMoveLeft  &= (Direction != Left)  || !bDidHit;
		_bCanMoveRight &= (Direction != Right) || !bDidHit;
	}

	bCanMoveLeft  = _bCanMoveLeft;
	bCanMoveRight = _bCanMoveRight;
}
