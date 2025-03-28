// Fill out your copyright notice in the Description page of Project Settings.


#include "Breakable/BreakableActor.h"

#include "Components/CapsuleComponent.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Items/Treasure.h"

ABreakableActor::ABreakableActor()
{
	PrimaryActorTick.bCanEverTick = false;
	
	GeometryCollection = CreateDefaultSubobject<UGeometryCollectionComponent>("GeometryCollection");
		SetRootComponent(GeometryCollection);
	GeometryCollection->SetGenerateOverlapEvents(true);	//Generate overlap events on default
	GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore); //Ignore camera
	GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore); //Ignore pawn
	Capsule = CreateDefaultSubobject<UCapsuleComponent>("Capsule");
		Capsule->SetupAttachment(GetRootComponent());
		Capsule->SetCollisionResponseToAllChannels(ECR_Ignore);
		Capsule->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	
}

void ABreakableActor::BeginPlay()
{
	Super::BeginPlay();
}

void ABreakableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABreakableActor::GetHit_Implementation(const FVector& ImpactPoint)
{
	UWorld* World = GetWorld();
	if (World && TreasureClass)
	{
		FVector Location = GetActorLocation();
		Location.Z += 75.f;
		//TreasureClass, so we can set in BP a BPClass to be spawned
		World->SpawnActor<ATreasure>(TreasureClass, Location, GetActorRotation());
	}
}

