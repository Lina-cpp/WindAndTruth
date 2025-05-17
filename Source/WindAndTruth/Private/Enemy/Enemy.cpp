// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"
#include "AIController.h"
#include "Characters/PlayerCharacterBase.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/AttributeComponent.h"
#include "HUD/HealthBarComponent.h"
#include "Items/Weapons/Weapon.h"

#include "Navigation/PathFollowingComponent.h"
#include "Perception/PawnSensingComponent.h"



AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
	
	//Mesh collision settings
	GetMesh()->SetGenerateOverlapEvents(true);
	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore); //ignore camera so there will not be any glitchy zooms

	//widgets and components
	HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>("Health Bar");
		HealthBarWidget->SetupAttachment(GetRootComponent());

	//Rotations for enemy movement, so they will rotate in side they are moving
	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	//PawnSensing Component ("eyes of npc")
	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>("PawnSensing");
		PawnSensing->SightRadius = 4000.f;
		PawnSensing->SetPeripheralVisionAngle(45.f);
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	/*	Declaring Delegate for OnSeePawn	*/
	if (PawnSensing) PawnSensing->OnSeePawn.AddDynamic(this, &AEnemy::PawnSeen);
	InitializeEnemy();
	Tags.Add(FName("Enemy"));

}


void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if (IsDead()) return;	//if enemy is already dead, don't do any checks
	
	if (EnemyState > EEnemyState::EES_Patrolling)
	{
		CheckCombatTarget();		
	}
	else
	{
		CheckPatrolTarget();	
	}

}


void AEnemy::SpawnDefaultWeapon()
{
	UWorld* World = GetWorld();
	if (World && WeaponClass)
	{
		AWeapon* DefaultWeapon = World->SpawnActor<AWeapon>(WeaponClass); //spawn Weapon on BeginPlay
		DefaultWeapon->Equip(GetMesh(), FName("RightHandSocket"), this, this); //equip weapon (aatach to socket)
		EquippedWeapon = DefaultWeapon;
	}
}



/**
 *	Navigation
**/

//Callback function for timer
void AEnemy::PatrolTimerFinished()
{
	MoveToTarget(PatrolTarget);
}

void AEnemy::PawnSeen(APawn* SeenPawn)
{
	const bool bShouldChaseTarget =
		EnemyState != EEnemyState::EES_Dead &&
		EnemyState != EEnemyState::EES_Chasing &&
		EnemyState < EEnemyState::EES_Chasing &&
		SeenPawn->ActorHasTag(FName("EngageableTarget"));

	if (bShouldChaseTarget)
	{
		CombatTarget = SeenPawn; //Set combat Target to SeenPawn(Player)
		ClearPatrolTimer(); //Stop patrolling
		ChaseTarget(); //Chase Target
	}
	//UE_LOG(LogTemp, Warning, TEXT("PawnSeen"));
}


AActor* AEnemy::ChoosePatrolTarget()
{
	
	//Prevent from picking the same patrol point again
	TArray<AActor*> ValidTargets;
	for (AActor* Target : PatrolTargets) //for loop for amount of TargetPatrolPoints
	{
		if (Target != PatrolTarget) //check if Target is not the same as PatrolTarget
		{
			ValidTargets.AddUnique(Target);
		}	
	}
	
	//Get random target from ValidTargets
	const int32 NumPatrolTargets = ValidTargets.Num(); //Num of elements, indexing starts from 0
	if (NumPatrolTargets > 0)
	{
		const int32 TargetSelection = FMath::RandRange(0, NumPatrolTargets - 1); //so -1
		return ValidTargets[TargetSelection]; //Save PatrolTarget as Actor
	}

	return nullptr;

}

void AEnemy::MoveToTarget(AActor* Target)
{
	if (EnemyController == nullptr || Target == nullptr) return; //if controller or target is null, leave function
	FAIMoveRequest MoveRequest;				//Create Request for AIMoveTo
	MoveRequest.SetGoalActor(Target);	//Set Destination
	MoveRequest.SetAcceptanceRadius(60.f);	//Radius of the dest. point that will count as destination
	EnemyController->MoveTo(MoveRequest);
}

void AEnemy::CheckPatrolTarget()
{
	//Movement between TargetPoints
	if (InTargetRange(PatrolTarget, PatrolRadius)) //check if enemy reached patrol point
	{
		PatrolTarget = ChoosePatrolTarget(); //find new point and Set PatrolTarget
		const float WaitTime = FMath::RandRange(MinMoveWait, MaxMoveWait);
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::PatrolTimerFinished, WaitTime); // Move to the new point after delay
	}
}






/**
 *	Combat
**/

void AEnemy::Attack()
{
	Super::Attack();
	if (CombatTarget == nullptr) return;

	EnemyState = EEnemyState::EES_Engaged;
	PlayAttackMontage();
}

void AEnemy::AttackEnd()
{
	EnemyState = EEnemyState::EES_NoState;
	CheckCombatTarget(); //check what to do next
	
}


void AEnemy::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	Super::GetHit_Implementation(ImpactPoint, Hitter);
	if (!IsDead()) ShowHealthBar();
	ClearPatrolTimer();
	ClearAttackTimer();
	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
	StopAttackMontage();
}

float AEnemy::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	class AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage(DamageAmount);
	CombatTarget = EventInstigator->GetPawn(); //Store dmg causer (player) to combat target
	if (IsInsideAttackRadius())
	{
		EnemyState = EEnemyState::EES_Attacking;
	}
	else if (IsOutsideAttackRadius())
	{
		ChaseTarget();
	}

	return DamageAmount;
}


void AEnemy::HandleDamage(float DamageAmount)
{
	Super::HandleDamage(DamageAmount);

	if (Attributes && HealthBarWidget) //check if attributes is valid (component with health)
	{
		HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent()); //update widget with matching %
	}
}


void AEnemy::CheckCombatTarget()
{
	if (IsOutsideCombatRadius()) /*	Check if Enemy is in Combat Radius, if not - stop following and hide Healthbar	*/
	{
		ClearAttackTimer();
		LoseInterest(); //Set CombatTarget to null and hide HealthBar
		if (!IsEngaged())
		{
			StartPatrolling(); //Change MoveSpeed and Go to patrolling targets
		}
	}
	else if (IsOutsideAttackRadius() && !IsChasing()) /*	chasing player		*/
	{
		ClearAttackTimer();
		if (!IsEngaged()) ChaseTarget(); //chase target if not engaged
	}
	else if (CanAttack()) /*	attacking player	*/
	{
		StartAttackTimer();
	}
}



/*
 * Montages
*/



void AEnemy::Die()
{
	Super::Die();
	
	EnemyState = EEnemyState::EES_Dead; //set enemy to dead (prevents playing hit animation)
	ClearAttackTimer();
	HideHealthBar();
	DisableCapsule();
	SetLifeSpan(DeathLifeSpan); //Destroy actor after 5s of being dead
	GetCharacterMovement()->bOrientRotationToMovement = false;
	PlayDeathSound(GetActorLocation());
	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
	
	//Rolling for Weapon Drop and destroying
	if (EquippedWeapon)
	{
		float DropRoll = FMath::FRand();
		if (DropRoll <= EquippedWeapon->DropChance)
		{
			if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString(TEXT("U got The Weapon!")));
			GetWorld()->SpawnActor<AWeapon>(WeaponClass, GetActorLocation(), FRotator(0.f, 0.f, 0.f));
		}
		else
		{
			if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString(TEXT("No weapon!")));
		}

		
		EquippedWeapon->SetLifeSpan(5.f);
	}
}



/*
*	HealthBar
*/

void AEnemy::HideHealthBar()
{
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(false);
	}
}

void AEnemy::ShowHealthBar()
{
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(true);
	}
}

/*
*	Bool Functions
*/

bool AEnemy::InTargetRange(AActor* Target, double Radius)
{
	if (Target == nullptr) return false;
	//Distance between Enemy and Player
	const double DistanceToTarget = (Target->GetActorLocation() - GetActorLocation()).Size();
	
	return DistanceToTarget <= Radius;
}

bool AEnemy::IsOutsideCombatRadius()
{
	return !InTargetRange(CombatTarget, CombatRadius);
}

bool AEnemy::IsOutsideAttackRadius()
{
	return !InTargetRange(CombatTarget, AttackRadius);
}

bool AEnemy::IsInsideAttackRadius()
{
	return InTargetRange(CombatTarget, AttackRadius);
}

bool AEnemy::IsChasing()
{
	return EnemyState == EEnemyState::EES_Chasing;
}

bool AEnemy::IsAttacking()
{
	return EnemyState == EEnemyState::EES_Attacking;
}

bool AEnemy::IsDead()
{
	return EnemyState == EEnemyState::EES_Dead;
}

bool AEnemy::IsEngaged()
{
	return EnemyState == EEnemyState::EES_Engaged;
}


bool AEnemy::CanAttack()
{
	bool bCanAttack =
		IsInsideAttackRadius() &&
		!IsAttacking() &&
		!IsDead() &&
		!IsEngaged();

	return bCanAttack;
}


/*
*	Enemy Behavior
*/


void AEnemy::InitializeEnemy()
{
	EnemyController = Cast<AAIController>(GetController()); //cast to AI controller with get controller
	SpawnDefaultWeapon(); /*	Spawning Weapon and Attaching to Character	*/
	MoveToTarget(PatrolTarget);
	
	/*	Check HP and set bar correctly	*/
	if (Attributes && HealthBarWidget)
	{
		HideHealthBar(); // Hide HealthBar from Screen
		//Good way to prevent optimization problems, should be creating HP widget on hit, and calculate hp
		//In case of a lot of npc on map this can turn into Dragons Dogma 2 problem
		HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent()); //calculate hp % to display properly
	}
}


void AEnemy::LoseInterest()
{
	CombatTarget = nullptr; //If player is further than Radius, lose interest and hide healthbar
	HideHealthBar();
}

void AEnemy::StartPatrolling()
{
	EnemyState = EEnemyState::EES_Patrolling; //back to patrolling on losing interest
	GetCharacterMovement()->MaxWalkSpeed = PatrolingSpeed; //Set Movement back to Patrolling Speed
	MoveToTarget(PatrolTarget); //Back to Patrol Target
}

void AEnemy::ChaseTarget()
{
	//Outside attack range but in chase radius
	EnemyState = EEnemyState::EES_Chasing;
	GetCharacterMovement()->MaxWalkSpeed = ChasingSpeed;
	
	MoveToTarget(CombatTarget);
}

void AEnemy::ClearPatrolTimer()
{
	GetWorldTimerManager().ClearTimer(PatrolTimer);
}

void AEnemy::ClearAttackTimer()
{
	GetWorldTimerManager().ClearTimer(AttackTimer);
}

void AEnemy::StartAttackTimer()
{
	//Call Attack() when Timer Finished
	EnemyState = EEnemyState::EES_Attacking;
	const float AttackTime = FMath::RandRange(AttackMin, AttackMax);
	GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, AttackTime);
	
}
