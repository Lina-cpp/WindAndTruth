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
	virtual void AttackEnd() override;
	
	void CheckPatrolTarget();
	void CheckCombatTarget();

	virtual bool CanAttack() override;
	virtual void Die() override;
	virtual void HandleDamage(float DamageAmount) override;

	virtual int32 PlayDeathMontage() override;

	UPROPERTY(EditAnywhere, Category = Combat)
	float DeathLifeSpan = 5.f;

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
*	Enums
*/

	//DeathPose enum, to play right anim
	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<EDeathPose> DeathPose;
	UPROPERTY(BlueprintReadOnly)
	EEnemyState EnemyState = EEnemyState::EES_Patrolling;




	
private:

/** AI Behavior **/
	void HideHealthBar();
	void ShowHealthBar();
	
	void LoseInterest();
	void StartPatrolling();
	void ChaseTarget();

	void ClearPatrolTimer();
	
	bool IsOutsideCombatRadius();
	bool IsOutsideAttackRadius();
	bool IsInsideAttackRadius();

	bool IsChasing();
	bool IsAttacking();
	bool IsDead();
	bool IsEngaged();

	

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
	double CombatRadius = 1000.f;
	UPROPERTY(EditAnywhere)
	double AttackRadius = 150.f;

	void StartAttackTimer();
	void ClearAttackTimer();
	
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


