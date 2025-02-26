// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawns/PosesPawns.h"

APosesPawns::APosesPawns()
{
	PrimaryActorTick.bCanEverTick = true;
}

void APosesPawns::BeginPlay()
{
	Super::BeginPlay();
	
}

void APosesPawns::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

// Called to bind functionality to input
void APosesPawns::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

