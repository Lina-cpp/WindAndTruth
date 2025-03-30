// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "Enemy.generated.h"

class UAnimMontage;
class UAttributeComponent;
class UHealthBarComponent;

UCLASS()
class WINDANDTRUTH_API AEnemy : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	AEnemy();
	virtual void Tick(float DeltaTime) override;

	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override; //PlayerInputs
	void DirectionalHitReact(const FVector& ImpactPoint); //Calculate from which side got hit
	virtual void GetHit_Implementation(const FVector& ImpactPoint) override; //override interface function
	//TakeDamage override (Actor virtual function)
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	
protected:
	virtual void BeginPlay() override;
	
/**
*	Play montage functions
**/
	void PlayHitReactMontage(const FName SectionName);

private:
/**
*	Widgets and Components
**/
	UPROPERTY(VisibleAnywhere)
	UAttributeComponent* Attributes;

	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* HealthBarWidget;


	
/**
* UAnimation Montages
**/
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* HitReactMontage;
	UPROPERTY(EditDefaultsOnly, Category = Sounds)
	USoundBase* HitSound;
	UPROPERTY(EditAnywhere, Category = VFX)
	UParticleSystem* HitParticle;

};

