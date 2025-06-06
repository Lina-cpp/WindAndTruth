// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Weapon.generated.h"

class UBoxComponent;
class USoundBase;

UCLASS()
class WINDANDTRUTH_API AWeapon : public AItem
{
	GENERATED_BODY()
public:
	AWeapon(); //Constructor
	
	void Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator);
	void AttachMeshToSocket(USceneComponent* InParent, FName InSocketName);
	void DisableSphereCollision();
	void DeactivatePickupEffect();
	void PlayEquipSound();

	TArray<AActor*> IgnoreActors; //Array of actors for weapon to ignore. Used to prevent multiple hits from one swing

	UPROPERTY(EditAnywhere, meta = (ClampMin = 0, ClampMax = 1))
	float DropChance = 0.f;
	
protected:
	void BeginPlay() override;
	
	UFUNCTION() //WeaponBox Overlap
	void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION(BlueprintImplementableEvent)
	void CreateFields(const FVector& FieldLocation);	//Function for breaking stuff

private:
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
		USoundBase* EquipSound;
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
		UBoxComponent* WeaponBox;

	//Scene components for box trace - get start and the end of the sword
	UPROPERTY(VisibleAnywhere)
		USceneComponent* BoxTraceStart;
	UPROPERTY(VisibleAnywhere)
		USceneComponent* BoxTraceEnd;
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	float WeaponDamage = 20;

	void BoxTrace(FHitResult& BoxHit);
	void ExecuteGetHit(FHitResult BoxHit);
	bool ActorIsSameType(AActor* OtherActor);

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	FVector BoxTraceExtent = FVector(5.f);
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	bool bShowBoxDebug = false;
	
/*
*	Setters & Getters
*/
public:
	
	FORCEINLINE UBoxComponent* GetWeaponBox() const {return WeaponBox;}
	
};
