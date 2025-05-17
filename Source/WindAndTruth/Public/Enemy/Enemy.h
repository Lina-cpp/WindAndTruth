// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"

#include "Enemy.generated.h"

class UHealthBarComponent;
class UPawnSensingComponent;

UCLASS()
class WINDANDTRUTH_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AEnemy();

	/*	<AActor>	*/
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	
	/*	<IHitInterface> */
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	
protected:
	/*	<AActor>	*/
	virtual void BeginPlay() override;
		


	/*	Combat */
	virtual void Attack() override;
	virtual void AttackEnd() override;

	virtual bool CanAttack() override;
	virtual void Die() override;
	virtual void HandleDamage(float DamageAmount) override;

	UPROPERTY(EditAnywhere, Category = Combat)
	float DeathLifeSpan = 5.f;
	
	UPROPERTY(BlueprintReadOnly)
	EEnemyState EnemyState = EEnemyState::EES_Patrolling;

	
private:
	/*	AI Behavior	*/

	void InitializeEnemy();
	void SpawnDefaultWeapon();

	AActor* ChoosePatrolTarget();
	
	UFUNCTION()
	void PawnSeen(APawn* SeenPawn);
	
	void CheckPatrolTarget();
	void CheckCombatTarget();
	void HideHealthBar();
	void ShowHealthBar();
	
	void LoseInterest();
	void StartPatrolling();
	void ChaseTarget();

	void ClearPatrolTimer();
	void PatrolTimerFinished();
	
	bool IsOutsideCombatRadius();
	bool IsOutsideAttackRadius();
	bool IsInsideAttackRadius();

	bool IsChasing();
	bool IsAttacking();
	bool IsDead();
	bool IsEngaged();

	void StartAttackTimer();
	void ClearAttackTimer();

	bool InTargetRange(AActor* Target, double Radius);
	void MoveToTarget(AActor* Target);
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AWeapon> WeaponClass; //enemy weapon
	
	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* HealthBarWidget;
	
	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent* PawnSensing;
	
	UPROPERTY(EditAnywhere)
	double CombatRadius = 1000.f;
	
	UPROPERTY(EditAnywhere)
	double AttackRadius = 200.f;

	
	UPROPERTY()
	class AAIController* EnemyController;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	AActor* PatrolTarget; 	//Current Patrol Target
	
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<AActor*> PatrolTargets; 	//All targets for AI to patrol
	
	UPROPERTY(EditAnywhere)
	double PatrolRadius = 200.f;

		
	UPROPERTY(EditAnywhere, Category = Combat)
	float PatrolingSpeed = 125.f;
	
	UPROPERTY(EditAnywhere, Category = Combat)
	float ChasingSpeed = 300.f;

	
	FTimerHandle PatrolTimer;
	
	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float MinMoveWait = 3.f;
	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float MaxMoveWait = 6.f;

	FTimerHandle AttackTimer;
	
	UPROPERTY(EditAnywhere, Category = Combat)
	float AttackMin = 0.5f;
	UPROPERTY(EditAnywhere, Category = Combat)
	float AttackMax = 1.f;

};


