// Fill out your copyright notice in the Description page of Project Settings.


#include "Block.h"

// Sets default values
ABlock::ABlock()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	SphereCollider = CreateDefaultSubobject<USphereComponent>(TEXT("CapsulseHorizontal"));
	SphereCollider->SetGenerateOverlapEvents(true);
	SphereCollider->SetSphereRadius(140.0f);
    SetRootComponent(SphereCollider);

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ABlock::BeginPlay()
{
	Super::BeginPlay();

	ParentActor = GetParentActor();
	
	SphereCollider->OnComponentBeginOverlap.AddDynamic(this, &ABlock::ABlock::OnBeginOverlap);
	SphereCollider->OnComponentBeginOverlap.AddDynamic(this, &ABlock::OnBeginOverlap);
}

// Called every frame
void ABlock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABlock::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == ParentActor) return;
	
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::White, TEXT("Overlap began."));
}

void ABlock::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (OtherActor == ParentActor) return;
	
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::White, TEXT("Overlap ended."));
}
