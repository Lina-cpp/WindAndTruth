// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "Interfaces/HitInterface.h"
#include "Characters/CharacterTypes.h" //include for ENUM

#include "Enemy.generated.h"

class UAnimMontage;
class UAttributeComponent;
class UHealthBarComponent;
class UPawnSensingComponent;

UCLASS()
class WINDANDTRUTH_API AEnemy : public ABaseCharacter, public IHitInterface
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
	
	
	void CheckPatrolTarget();
	void CheckCombatTarget();

	/** 
*	Navigation
**/
	bool InTargetRange(AActor* Target, double Radius);
	void MoveToTarget(AActor* Target);

	AActor* ChoosePatrolTarget();
	
	FTimerHandle PatrolTimer;
	void PatrolTimerFinished();

	UFUNCTION()
	void PawnSeen(APawn* SeenPawn);




	
	//DeathPose enum, to play right anim
	UPROPERTY(BlueprintReadOnly)
	EDeathPose DeathPose = EDeathPose::EDP_Alive;

	EEnemyState EnemyState = EEnemyState::EES_Patrolling;
	
/**
*	Play montage functions
**/
	void PlayHitReactMontage(const FName SectionName);
	void Die();


	
private:

/**
*	Combat Target And Radius
**/
	UPROPERTY()
	AActor* CombatTarget;
	UPROPERTY(EditAnywhere)
	double CombatRadius = 500.f;
	UPROPERTY(EditAnywhere)
	double AttackRadius = 150.f;

	
/** 
*	Navigation
**/
	UPROPERTY()
	class AAIController* EnemyController;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
		AActor* PatrolTarget; 	//Current Patrol Target
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
		TArray<AActor*> PatrolTargets; 	//All targets for AI to patrol
	UPROPERTY(EditAnywhere)
		double PatrolRadius = 200.f;
	UPROPERTY(EditAnywhere, Category = "AI Navigation")
		float MinMoveWait = 3.f;
	UPROPERTY(EditAnywhere, Category = "AI Navigation")
		float MaxMoveWait = 6.f;




	
/**
*	Widgets and Components
**/
	UPROPERTY(VisibleAnywhere)
	UAttributeComponent* Attributes;

	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* HealthBarWidget;
	
	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent* PawnSensing;


	
/**
* UAnimation Montages
**/
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* HitReactMontage;
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* DeathMontage;
	UPROPERTY(EditDefaultsOnly, Category = Sounds)
	USoundBase* HitSound;
	UPROPERTY(EditAnywhere, Category = VFX)
	UParticleSystem* HitParticle;

};


