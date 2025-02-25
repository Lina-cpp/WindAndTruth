// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Item.h"
#include "DrawDebugHelpers.h"
#include "WindAndTruth/DebugMacros.h"



AItem::AItem()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AItem::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld(); //Make UWorld Pointer Var to get world
	FVector Location = GetActorLocation();
	FVector Forward = GetActorForwardVector();

	DRAW_SPHERE(Location) //doesn't need ; because there is one ";" in macro
	//DRAW_LINE(Location, Location + Forward * 100.f)
	//DRAW_POINT(Location + Forward * 100.f)
	DRAW_VECTOR(Location, Location + Forward * 100.f);
	DRAW_SPHERE2 (Location, FColor::Emerald)


}

void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

