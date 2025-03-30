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
	
	TArray<AActor*> IgnoreActors; //Array of actors for weapon to ignore. Used to prevent multiple hits from one swing
	
protected:
	void BeginPlay() override;
	
	//override of parent functions
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

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

/*
*	Setters & Getters
*/
public:
	
	FORCEINLINE UBoxComponent* GetWeaponBox() const {return WeaponBox;}
	
};
