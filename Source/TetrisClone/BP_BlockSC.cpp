// Fill out your copyright notice in the Description page of Project Settings.


#include "BP_BlockSC.h"

// Sets default values for this component's properties
UBP_BlockSC::UBP_BlockSC()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));

	CapsuleHorizontal = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsulseHorizontal"));
	CapsuleHorizontal->SetupAttachment(StaticMeshComponent);
	CapsuleHorizontal->SetGenerateOverlapEvents(true);
	CapsuleHorizontal->SetCapsuleHalfHeight(96.0f);
	CapsuleHorizontal->SetCapsuleRadius(48.0f);

	CapsuleVertical = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleVertical"));
	CapsuleVertical->SetupAttachment(StaticMeshComponent);
	CapsuleVertical->SetGenerateOverlapEvents(true);
	CapsuleVertical->SetCapsuleHalfHeight(96.0f);
	CapsuleVertical->SetCapsuleRadius(48.0f);
}


// Called when the game starts
void UBP_BlockSC::BeginPlay()
{
	Super::BeginPlay();

	// ...
	CapsuleHorizontal->OnComponentBeginOverlap.AddDynamic(this, &UBP_BlockSC::OnBeginOverlap);
	CapsuleVertical->OnComponentBeginOverlap.AddDynamic(this, &UBP_BlockSC::OnBeginOverlap);

	CapsuleHorizontal->OnComponentEndOverlap.AddDynamic(this, &UBP_BlockSC::OnEndOverlap);
	CapsuleVertical->OnComponentEndOverlap.AddDynamic(this, &UBP_BlockSC::OnEndOverlap);
}


// Called every frame
void UBP_BlockSC::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UBP_BlockSC::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::White, TEXT("Overlap began."));
}

void UBP_BlockSC::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::White, TEXT("Overlap ended."));
}