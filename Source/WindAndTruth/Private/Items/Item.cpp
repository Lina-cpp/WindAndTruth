// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Item.h"
#include "DrawDebugHelpers.h"
#include "WindAndTruth/DebugMacros.h"
#include "Components/SphereComponent.h"
#include "Characters/PlayerCharacterBase.h"
#include "NiagaraComponent.h"



AItem::AItem()
{
	PrimaryActorTick.bCanEverTick = true;
	
	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMeshComponent")); //Create statis mesh component
		RootComponent = ItemMesh;
	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
		Sphere->SetupAttachment(RootComponent);

	PickupEffect = CreateDefaultSubobject<UNiagaraComponent>("PickupEffect");
		PickupEffect->SetupAttachment(RootComponent);
}

void AItem::BeginPlay()
{
	Super::BeginPlay();
	//Bind Function to Delegate				On Overlap of THIS sphere, call Function
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlap);
	Sphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);
}

void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	RunningTime += DeltaTime;

	//Hovering Logic
	//Check if is in hovering state, if yes, hover - For weapon, State is changed in AWeapon::Equip
	if (ItemState == EItemState::EIS_Hovering)
	{
		AddActorWorldOffset(FVector(0.f, 0.f,TransformedSin()));	
	}
	
}


float AItem::TransformedSin()
{
	return Amplitude * FMath::Sin(RunningTime * TimeConstant);
}
float AItem::TransformedCosin()
{
	return Amplitude * FMath::Cos(RunningTime * TimeConstant);
}


/**
 * OverlapEvents 
**/
void AItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlayerCharacterBase* PlayerCharacterBase = Cast<APlayerCharacterBase>(OtherActor); //On Overlap cast to PlayerCharacter
	if (PlayerCharacterBase)
	{
		PlayerCharacterBase->SetOverlappingItem(this); //if valid, Get function from PlayerCharacter and call SetOverlapping item with THIS (this weapon)
		
	}
}
void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	APlayerCharacterBase* PlayerCharacterBase = Cast<APlayerCharacterBase>(OtherActor);
	if (PlayerCharacterBase)
	{
		PlayerCharacterBase->SetOverlappingItem(nullptr); // Clear weapon
	}
}

