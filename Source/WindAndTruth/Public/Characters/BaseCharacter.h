// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "BaseCharacter.generated.h"

class AWeapon;
class UAttributeComponent;
class UAnimMontage;


UCLASS()
class WINDANDTRUTH_API ABaseCharacter : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();
	virtual void Tick(float DeltaTime) override;

	
protected:
	virtual void BeginPlay() override;
	
/*
*	Combat & Weapons
*/
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	UFUNCTION(BlueprintCallable)
	void SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);
	
	virtual bool CanAttack();
	virtual void Attack();
	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd();
	bool IsAlive();
	virtual void Die();

	virtual void HandleDamage(float DamageAmount);
	
	//Pointer to weapon char ic currently using
	UPROPERTY(VisibleAnywhere, Category = Weapon) 
	AWeapon* EquippedWeapon;

	

/*
* SFX & VFX
*/
	void PlayHitSound(const FVector& ImpactPoint);
	void SpawnHitParticle(const FVector& ImpactPoint);
	void PlayDeathSound(const FVector& ImpactPoint);

	void PlayHitReactMontage(const FName SectionName);
	void DirectionalHitReact(const FVector& ImpactPoint); //Calculate from which side got hit

	void PlayMontageSection(UAnimMontage* Montage, const FName& SectionName);
	int32 PlayRandomMontageSection(UAnimMontage* Montage, const TArray<FName>& SectionNames);
	virtual int32 PlayAttackMontage();
	virtual int32 PlayDeathMontage();
	void DisableCapsule();
	
	void StopAttackMontage();
/**
*	Widgets and Components
**/
	UPROPERTY(VisibleAnywhere)
	UAttributeComponent* Attributes;

/*
* SFX & VFX
*/
private:
	UPROPERTY(EditDefaultsOnly, Category = "Effects | SFX")
	USoundBase* HitSound;
	UPROPERTY(EditAnywhere, Category = "Effects | VFX")
	UParticleSystem* HitParticle;
	UPROPERTY(EditDefaultsOnly, Category = "Effects | SFX")
	USoundBase* DeathSound;

/**
* Animation Montages
**/
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* AttackMontage;
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* HitReactMontage;
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* DeathMontage;
	UPROPERTY(EditDefaultsOnly, Category = Combat)
	TArray<FName> AttackMontageSections; //Array of all Montage Sections
	UPROPERTY(EditDefaultsOnly, Category = Combat)
	TArray<FName> DeathMontageSections;

};


