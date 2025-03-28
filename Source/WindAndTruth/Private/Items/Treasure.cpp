// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Treasure.h"
#include "Characters/PlayerCharacterBase.h"
#include "Kismet/GameplayStatics.h"

void ATreasure::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//if player overlap, play sound and destroy
	APlayerCharacterBase* PlayerCharacter = Cast<APlayerCharacterBase>(OtherActor);
	if (PlayerCharacter)
	{
		if (PickupSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, PickupSound, GetActorLocation());
		}
		Destroy();
	}
}
