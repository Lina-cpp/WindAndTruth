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



void AWeapon::BoxTrace(FHitResult& BoxHit)
{
	/*	Start&End locations for trace	*/
	const FVector Start = BoxTraceStart->GetComponentLocation();
	const FVector End = BoxTraceEnd->GetComponentLocation();
	
	TArray<AActor*> ActorsToIgnore;	//Array of actors to ignore
	ActorsToIgnore.Add(this); //Add this to array, so weapon will ignore itself
	ActorsToIgnore.Add(GetOwner());

	for (AActor* Actor : IgnoreActors) //loop will be executed once for each actor
	{
		ActorsToIgnore.AddUnique(Actor);
	}
	
	UKismetSystemLibrary::BoxTraceSingle(this,
		Start, End,
		BoxTraceExtent,
		BoxTraceStart->GetComponentRotation(),
		ETraceTypeQuery::TraceTypeQuery1,
		false,
		ActorsToIgnore,
		bShowBoxDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
		BoxHit,
		true
		);
	IgnoreActors.AddUnique(BoxHit.GetActor()); //Add Actor that got hit to Ignore, so we don't get multiple hits by one swing
}


void AWeapon::ExecuteGetHit(FHitResult BoxHit)
{
	//check if actor we hit has interface
	IHitInterface* HitInterface = Cast<IHitInterface>(BoxHit.GetActor());
	if (HitInterface) //check if cast is not failed
	{
		HitInterface->Execute_GetHit(BoxHit.GetActor(), BoxHit.ImpactPoint, GetOwner()); //since interface is native, call this to get it in cpp&bp
	}
}

bool AWeapon::ActorIsSameType(AActor* OtherActor)
{
	return GetOwner()->ActorHasTag(TEXT("Enemy")) && OtherActor->ActorHasTag(TEXT("Enemy"));
}

void AWeapon::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                           int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ActorIsSameType(OtherActor)) return;

	
	FHitResult BoxHit;
	BoxTrace(BoxHit);
	
	//check if actor that got hit is valid
	if (BoxHit.GetActor())
	{
		if (ActorIsSameType(BoxHit.GetActor())) return;
		
		UGameplayStatics::ApplyDamage(
			BoxHit.GetActor(), //actor that got hit
			WeaponDamage,//Weapons Damage to deal
			GetInstigator()->GetController(), //Get controller of pawn that owns weapon and apllied hit in this case - player
			this, //Actor that caused damage (this) weapon
			UDamageType::StaticClass() //Damage type class
			);
		
		ExecuteGetHit(BoxHit);
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


void AWeapon::Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator)
{
	//Enum for how to Attach to target
	ItemState = EItemState::EIS_Equipped;
	SetOwner(NewOwner);
	SetInstigator(NewInstigator);
	AttachMeshToSocket(InParent, InSocketName);
	DisableSphereCollision();
	PlayEquipSound();
	DeactivatePickupEffect();
}


void AWeapon::PlayEquipSound()
{
	if (EquipSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			EquipSound,
			GetActorLocation()
		);
	}
}

void AWeapon::DisableSphereCollision()
{
	if (Sphere)
	{
		//When we pick up weapon, turn off sphere collision, so it won't call overlap events on attacks
		Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AWeapon::DeactivatePickupEffect()
{
	if (PickupEffect)
	{
		PickupEffect->Deactivate(); //Deactivate on weapon equip
	}
}