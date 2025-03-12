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

	auto CurrentLocation = GetActorLocation();
	auto Components = GetComponents();

	float MaxNegativeZDifference = 0.0f;
	UActorComponent* LowestComponent {};
	
	// for (auto Component : Components)
	// {
	// 	auto SMComp = Cast<UStaticMeshComponent>(Component);
	//
	// 	auto Location = SMComp->GetComponentLocation();
	// 	auto ZDifference = CurrentLocation.Z - Location.Z;
	//
	// 	if (ZDifference < MaxNegativeZDifference)
	// 	{
	// 		LowestComponent = SMComp;
	// 	}
	// }
	//
	// if (LowestComponent)
	// {
	// 	auto Str = FString::Printf(TEXT("%f"), MaxNegativeZDifference);
	// 	GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::White, Str);
	// }
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

void ATetrisPiece::OnComponentHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	bCanMove = false;
}

void ATetrisPiece::LeftRight(const FInputActionValue& Value)
{
	if (!bCanMove) return;

	float ActionValue = Value.Get<float>();
	float Direction = FMath::Sign(ActionValue);
	
	auto CurrentLocation = GetActorLocation();
	SetActorLocation({CurrentLocation.X + Direction * 100.0f, CurrentLocation.Y, CurrentLocation.Z + 5});
}

void ATetrisPiece::Down()
{
	if (!bCanMove) return;

	auto CurrentLocation = GetActorLocation();
	SetActorLocation({CurrentLocation.X, CurrentLocation.Y, CurrentLocation.Z - 100.0f});
}

void ATetrisPiece::Rotate()
{
	if (!bCanMove) return;
	
	auto Rotation = FRotator(-90.0f, 0.0f, 0.0f);
	
	// auto StartQuat = GetActorRotation().Quaternion();
	// auto EndQuat = StartQuat * Rotation.Quaternion();
	// auto SlerpedQuat = FQuat::Slerp(StartQuat, EndQuat, 1.0f);
	// SetActorRotation(SlerpedQuat);
	
	AddActorLocalRotation(Rotation);
}

void ATetrisPiece::OnDropTimeout()
{
	auto CurrentLocation = GetActorLocation();
	
	auto NewLocation = FVector(CurrentLocation.X, CurrentLocation.Y, CurrentLocation.Z - 100.0f);
	SetActorLocation(NewLocation);
}

