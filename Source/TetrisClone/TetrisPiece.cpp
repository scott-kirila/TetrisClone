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
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATetrisPiece::Move);
		EnhancedInputComponent->BindAction(RotateAction, ETriggerEvent::Triggered, this, &ATetrisPiece::Rotate);
	}
}

void ATetrisPiece::Move(const FInputActionValue& Value)
{
	auto CurrentLocation = GetActorLocation();
	SetActorLocation({CurrentLocation.X, CurrentLocation.Y, CurrentLocation.Z + 5});
}

void ATetrisPiece::Rotate(const FInputActionValue& Value)
{
}

