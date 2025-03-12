// Fill out your copyright notice in the Description page of Project Settings.


#include "TetrisPiece.h"

#include "EnhancedInputComponent.h"

// Sets default values
ATetrisPiece::ATetrisPiece()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	
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
}

// Called to bind functionality to input
void ATetrisPiece::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	auto EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	if (EnhancedInputComponent)
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATetrisPiece::Move);

		if (bCanRotate)
		{
			EnhancedInputComponent->BindAction(RotateAction, ETriggerEvent::Started, this, &ATetrisPiece::Rotate);
		}
	}
}

void ATetrisPiece::OnComponentHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	bCanMove = false;
}

void ATetrisPiece::Move(const FInputActionValue& Value)
{
	if (!bCanMove) return;
	
	auto CurrentLocation = GetActorLocation();
	SetActorLocation({CurrentLocation.X, CurrentLocation.Y, CurrentLocation.Z + 5}, true);
}

void ATetrisPiece::Rotate(const FInputActionValue& Value)
{
	if (!bCanMove) return;
	
	auto Rotation = FRotator(-90.0f, 0.0f, 0.0f);
	AddActorLocalRotation(Rotation);
}

void ATetrisPiece::OnDropTimeout()
{
	auto CurrentLocation = GetActorLocation();

	
	
	auto NewLocation = FVector(CurrentLocation.X, CurrentLocation.Y, CurrentLocation.Z - 100.0f);
	SetActorLocation(NewLocation);
}

