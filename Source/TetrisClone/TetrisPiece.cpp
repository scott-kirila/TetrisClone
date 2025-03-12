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

	if (!bCanMove && GetWorldTimerManager().IsTimerActive(DropTimer))
	{
		GetWorldTimerManager().ClearTimer(DropTimer);
	}

	auto CurrentLocation = GetActorLocation();
	auto Components = GetComponents();

	float MaxNegativeZDifference = 0.0f;
	UActorComponent* LowestComponent {};
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
	SetActorLocation({CurrentLocation.X + Direction * 100.0f, CurrentLocation.Y, CurrentLocation.Z});
}

void ATetrisPiece::Down()
{
	if (!bCanMove) return;

	auto CurrentLocation = GetActorLocation();
	SetActorLocation({CurrentLocation.X, CurrentLocation.Y, CurrentLocation.Z - 100.0f});

	CheckForStop();
}

void ATetrisPiece::Rotate()
{
	if (!bCanMove) return;

	auto Rotation = FRotator(-90.0f, 0.0f, 0.0f);
	AddActorLocalRotation(Rotation);

	CheckForStop();
}

void ATetrisPiece::OnDropTimeout()
{
	if (!bCanMove) return;
	
	auto CurrentLocation = GetActorLocation();
	
	auto NewLocation = FVector(CurrentLocation.X, CurrentLocation.Y, CurrentLocation.Z - 100.0f);
	SetActorLocation(NewLocation);

	CheckForStop();
}

void ATetrisPiece::CheckForStop()
{
	TArray<USceneComponent*> Meshes;
	GetComponents(Meshes, true);
	
	for (auto& Mesh : Meshes)
	{
		FVector Start = Mesh->GetComponentLocation() - FVector::UpVector * 45.0f;
		FVector End = Start - FVector::UpVector * TraceDistance;
		FHitResult OutHit;
		DrawDebugLine(GetWorld(), Start, End, FColor::Green, true, 1, 0, 5);
		GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_WorldStatic);

		if (OutHit.bBlockingHit && OutHit.Component->GetOwner() != this)
		{
			bCanMove = false;
			bCanRotate = false;
			GetWorldTimerManager().ClearTimer(DropTimer);
		}
	}
}
