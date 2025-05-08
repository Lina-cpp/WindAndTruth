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

	GeometryCollection->OnChaosBreakEvent.AddDynamic(this, &ABreakableActor::OnBreakEvent);
}





void ABreakableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABreakableActor::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	if (GEngine) GEngine->AddOnScreenDebugMessage(4, 5.f, FColor::Red, TEXT("CPP GetHit"));		
}

//spawn collectibles when object is damaged, so item will always spawn
void ABreakableActor::OnBreakEvent(const FChaosBreakEvent& BreakEvent)
{
	GEngine->AddOnScreenDebugMessage(4, 5.f, FColor::Red, TEXT("OnBreakEvent"));
	if (bBroken) return;
	bBroken = true;
	UWorld* World = GetWorld();
	if (World && TreasureClasses.Num() > 0)
	{
		FVector Location = GetActorLocation();
		Location.Z += 75.f;

		const int32 Selection = FMath::RandRange(0, TreasureClasses.Num() - 1); //Random int to spawn random treasure
		//TreasureClass, so we can set in BP a BPClass to be spawned
		World->SpawnActor<ATreasure>(TreasureClasses[Selection], Location, GetActorRotation());
	}
}

