// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapons/Weapon.h"
#include "Characters/PlayerCharacterBase.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Interfaces/HitInterface.h"
#include "NiagaraComponent.h"

AWeapon::AWeapon()
{
	//Collision box for Sword
	WeaponBox = CreateDefaultSubobject<UBoxComponent>("WeaponBox");
		WeaponBox->SetupAttachment(GetRootComponent());
		WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision); //No collision - Attacks will enable collision
		WeaponBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);//Set Coll overlap for all
		WeaponBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore); //Ignore collision with PAWN

	BoxTraceStart = CreateDefaultSubobject<USceneComponent>("BoxTraceStart");
		BoxTraceStart->SetupAttachment(GetRootComponent());
	BoxTraceEnd = CreateDefaultSubobject<USceneComponent>("BoxTraceEnd");
		BoxTraceEnd->SetupAttachment(GetRootComponent());
}


void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	//When wepon box begin overlap, use OnBoxOverlap()
	WeaponBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnBoxOverlap);
}




/**
 * Overlap Functions
**/
void AWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                              UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}
void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnSphereEndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}


void AWeapon::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	const FVector Start = BoxTraceStart->GetComponentLocation(); //Get StartComponent World Location and save it
	const FVector End = BoxTraceEnd->GetComponentLocation(); //Get EndComponent World Location and save it
	
	TArray<AActor*> ActorsToIgnore;	//Array of actors to ignore
	ActorsToIgnore.Add(this); //Add this to array, so weapon will ignore itself

	for (AActor* Actor : IgnoreActors) //loop will be executed once for each actor
	{
		ActorsToIgnore.AddUnique(Actor);
	}
	
	FHitResult BoxHit; //Out parameter, will be filled with info
	UKismetSystemLibrary::BoxTraceSingle(this,
		Start, End,
		FVector(5.f, 5.f, 5.f),
		BoxTraceStart->GetComponentRotation(),
		ETraceTypeQuery::TraceTypeQuery1,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		BoxHit,
		true
		);
	if (BoxHit.GetActor())
	{
		//check if actor we hit has interface
		IHitInterface* HitInterface = Cast<IHitInterface>(BoxHit.GetActor());
		if (HitInterface) //check if cast is not failed
		{
			HitInterface->Execute_GetHit(BoxHit.GetActor(), BoxHit.ImpactPoint); //since interface is native, call this to get it in cpp&bp
		}
		
		IgnoreActors.AddUnique(BoxHit.GetActor()); //Add Actor that got hit to Ignore, so we don't get multiple hits by one swing
		CreateFields(BoxHit.ImpactPoint);
	}
}




/*
 * Other Functions
 */

//Function that AttachesMesh to socket - used to attach weapon to hand and spine socket
void AWeapon::AttachMeshToSocket(USceneComponent* InParent, FName InSocketName)
{
	FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
	ItemMesh->AttachToComponent(InParent, TransformRules, InSocketName);
}

void AWeapon::Equip(USceneComponent* InParent, FName InSocketName)
{
	//Enum for how to Attach to target
	AttachMeshToSocket(InParent, InSocketName);
	ItemState = EItemState::EIS_Equipped;
	if (EquipSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			EquipSound,
			GetActorLocation()
			);
	}
	if (Sphere)
	{
		//When we pick up weapon, turn off sphere collision, so it won't call overlap events on attacks
		Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	if (PickupEffect)
	{
		PickupEffect->Deactivate(); //Deactivate on weapon equip
	}
}
