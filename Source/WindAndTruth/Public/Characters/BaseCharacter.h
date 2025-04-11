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

	UFUNCTION(BlueprintCallable)
	void SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);
	
protected:
	virtual void BeginPlay() override;
	
/*
*	Combat & Weapons
*/
	virtual bool CanAttack();
	virtual void Attack();
	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd();
	virtual void Die();
	
	//Pointer to weapon char ic currently using
	UPROPERTY(VisibleAnywhere, Category = Weapon) 
	AWeapon* EquippedWeapon;

	
/**
* Animation Montages
**/
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* AttackMontage;
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* HitReactMontage;
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* DeathMontage;
	
/**
*	Play montage functions
**/
	virtual void PlayAttackMontage();
	void PlayHitReactMontage(const FName SectionName);
	void DirectionalHitReact(const FVector& ImpactPoint); //Calculate from which side got hit


/**
*	Widgets and Components
**/
	UPROPERTY(VisibleAnywhere)
	UAttributeComponent* Attributes;

/*
* SFX & VFX
*/
	UPROPERTY(EditDefaultsOnly, Category = Sounds)
	USoundBase* HitSound;
	UPROPERTY(EditAnywhere, Category = VFX)
	UParticleSystem* HitParticle;
};
