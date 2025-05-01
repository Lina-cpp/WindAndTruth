// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "Characters/CharacterTypes.h" //include for ENUM

#include "Enemy.generated.h"

class UHealthBarComponent;
class UPawnSensingComponent;

UCLASS()
class WINDANDTRUTH_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AEnemy();
	virtual void Tick(float DeltaTime) override;

	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override; //PlayerInputs
	virtual void GetHit_Implementation(const FVector& ImpactPoint) override; //override interface function
	//TakeDamage override (Actor virtual function)
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	//virtual void Destroyed() override;
	
protected:
	virtual void BeginPlay() override;
	
/*
*	Combat
*/
	virtual void Attack() override;
	
	void CheckPatrolTarget();
	void CheckCombatTarget();

		
	virtual void Die() override;

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

	
/*
*	Montages
*/

	virtual void PlayAttackMontage() override;


	

/*
*	Enums
*/

	//DeathPose enum, to play right anim
	UPROPERTY(BlueprintReadOnly)
	EDeathPose DeathPose;
	UPROPERTY(BlueprintReadOnly)
	EEnemyState EnemyState = EEnemyState::EES_Patrolling;



	
private:

/** AI Behavior **/
	void HideHealthBar();
	void ShowHealthBar();
	
	void LoseInterest();
	void StartPatrolling();
	void ChaseTarget();
	
	bool IsOutsideCombatRadius();
	bool IsOutsideAttackRadius();
	bool IsInsideAttackRadius();

	bool IsChasing();
	bool IsAttacking();

	UPROPERTY(EditAnywhere, Category = Combat)
	float PatrolingSpeed = 125.f;
	UPROPERTY(EditAnywhere, Category = Combat)
	float ChasingSpeed = 300.f;
	
/**
*	Combat Target And Radius
**/
	UPROPERTY()
	AActor* CombatTarget;
	UPROPERTY(EditAnywhere)
	double CombatRadius = 500.f;
	UPROPERTY(EditAnywhere)
	double AttackRadius = 150.f;

	void StartAttackTimer();
	FTimerHandle AttackTimer;
	UPROPERTY(EditAnywhere, Category = Combat)
	float AttackMin = 0.5f;
	UPROPERTY(EditAnywhere, Category = Combat)
	float AttackMax = 1.f;

	
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

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AWeapon> WeaponClass; //enemy weapon

	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* HealthBarWidget;
	
	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent* PawnSensing;
	

};


