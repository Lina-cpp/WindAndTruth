// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"
#include "AIController.h"
#include "Characters/PlayerCharacterBase.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/AttributeComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

#include "HUD/HealthBarComponent.h"
#include "Runtime/AIModule/Classes/AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "Perception/PawnSensingComponent.h"

#include "WindAndTruth/DebugMacros.h"


AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
	
	//Mesh collision settings
	GetMesh()->SetGenerateOverlapEvents(true);
	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore); //ignore camera so there will not be any glitchy zooms
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

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

	if (PawnSensing)
	{
		PawnSensing->OnSeePawn.AddDynamic(this, &AEnemy::PawnSeen);
	}

	
	//Check HP and set bar correctly
	if (Attributes && HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(false);// Hide HealthBar from Screen
		//Good way to prevent optimization problems, should be creating HP widget on hit, and calculate hp
		//In case of a lot of npc on map this can turn into Dragons Dogma 2 problem
		HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent()); //calculate hp % to display properly
	}



	
	EnemyController = Cast<AAIController>(GetController()); //cast to AI controller with get controller
	MoveToTarget(PatrolTarget); //Call function to Move Enemy
	
}


void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
	if (EnemyState > EEnemyState::EES_Patrolling)
	{
		CheckCombatTarget();		
	}
	else
	{
		CheckPatrolTarget();	
	}
	

}


void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

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
	//UE_LOG(LogTemp, Warning, TEXT("PawnSeen"));
	//check if seen pawn has tag (only player has this tag) - Tag added in PlayerCharBase.cpp in BeginPlay
	if (EnemyState == EEnemyState::EES_Chasing) return; //if already in chase, leave function
	if (SeenPawn->ActorHasTag(FName("Player")))
	{
		GetWorldTimerManager().ClearTimer(PatrolTimer); //Clear Patrol timer - won't move towards another point
		GetCharacterMovement()->MaxWalkSpeed = 300.f; //Accelerate enemy
		CombatTarget = SeenPawn; //Set combat Target to SeenPawn(Player)

		if (EnemyState != EEnemyState::EES_Attacking) //if is not attacking
		{
			EnemyState = EEnemyState::EES_Chasing; //change state to chase
			MoveToTarget(CombatTarget); //Move Enemy to Combat target
			if (GEngine) GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Blue, FString(TEXT("Pawn Seen, now Chase player")));
		}
	}
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
	MoveRequest.SetAcceptanceRadius(15.f);	//Radius of the dest. point that will count as destination
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


/*
*	Bool Functions
*/


bool AEnemy::InTargetRange(AActor* Target, double Radius)
{
	if (Target == nullptr) return false;
	//Distance between Enemy and Player
	const double DistanceToTarget = (Target->GetActorLocation() - GetActorLocation()).Size();

	//Debug
	DRAW_SPHERE_SingleFrame(GetActorLocation())
	DRAW_SPHERE_SingleFrame(Target->GetActorLocation())
	
	return DistanceToTarget <= Radius;
}




/**
 *	Combat
**/

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint)
{
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(true); //show healthbar when enemy got hit
	}
	//After applying dmg, check is Character Alive and decide what anim to play
	if ( Attributes && Attributes->IsAlive())
	{
		DirectionalHitReact(ImpactPoint);
	}
	else
	{
		Die();
	}

	if (HitSound) UGameplayStatics::PlaySoundAtLocation(this, HitSound,ImpactPoint); //if valid, play sound at location
	if (HitParticle) UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticle, ImpactPoint);
}

float AEnemy::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	class AController* EventInstigator, AActor* DamageCauser)
{
	if (Attributes && HealthBarWidget) //check if attributes is valid (component with health)
	{
		Attributes->ReceiveDamage(DamageAmount); //Calculate damage based on input dmg and hp
		HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent()); //update widget with matching %
	}
	
	CombatTarget = EventInstigator->GetPawn(); //Store dmg causer (player) to combat target
	EnemyState = EEnemyState::EES_Chasing; //Change Enemy State and make them chase player
	MoveToTarget(CombatTarget);
	GetCharacterMovement()->MaxWalkSpeed = 300.f;
	
	return DamageAmount;
}



void AEnemy::CheckCombatTarget()
{
	//Losing Interest when out of range
	//Check if Enemy is in Combat Radius, if not - stop following and hide Healthbar
	if (!InTargetRange(CombatTarget, CombatRadius)) 
	{
		CombatTarget = nullptr; //If player is further than Radius, lose interest and hide healthbar
		if (HealthBarWidget)
		{
			HealthBarWidget->SetVisibility(false);
		}
		EnemyState = EEnemyState::EES_Patrolling; //back to patrolling on losing interest
		GetCharacterMovement()->MaxWalkSpeed = 125.f; //Set Movement back to Patrolling Speed
		MoveToTarget(PatrolTarget); //Back to Patrol Target

		if (GEngine) GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Blue, FString(TEXT("Lose Intereset")));
	}

	//chasing player
	else if (!InTargetRange(CombatTarget, AttackRadius) && EnemyState != EEnemyState::EES_Chasing)
	{
		//Outside attack range but in chase radius
		EnemyState = EEnemyState::EES_Chasing;
		GetCharacterMovement()->MaxWalkSpeed = 300.f;
		MoveToTarget(CombatTarget);

		if (GEngine) GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Blue, FString(TEXT("Chase player")));
	}

	//attacking player
	else if (InTargetRange(CombatTarget, AttackRadius) && EnemyState != EEnemyState::EES_Attacking)
	{
		//Inside attack range, so attack player
		EnemyState = EEnemyState::EES_Attacking;
		//TODO Play attack montage

		if (GEngine) GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Blue, FString(TEXT("Attack")));
	}
}
















/*
 * Montages
*/

void AEnemy::Die()
{
	
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DeathMontage)
	{
		AnimInstance->Montage_Play(DeathMontage); //play montage

		//get random anim to play
		const int32 Selection = FMath::RandRange(0,2); //random number (depends on how many anims I have)
		FName SectionName = FName(); //Clear FName, to overwrite in switch
		switch (Selection)
		{
		case 0:
			SectionName = 	FName("Death1");
			DeathPose = EDeathPose::EDP_Death1;
			break;
		case 1:
			SectionName = 	FName("Death2");
			DeathPose = EDeathPose::EDP_Death2;
			break;
		case 2:
			SectionName = 	FName("Death3");
			DeathPose = EDeathPose::EDP_Death3;
			break;
		case 3:
			SectionName = 	FName("Death4");
			DeathPose = EDeathPose::EDP_Death4;
			break;
		case 4:
			SectionName = 	FName("Death5");
			DeathPose = EDeathPose::EDP_Death5;
			break;
			
		default:
			break;
		}
		
		AnimInstance->Montage_JumpToSection(SectionName, DeathMontage);
	}
	
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(false); //Hide healthBar when enemy Diesw
	}

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision); //Disable Enemy Collision when is dead
	SetLifeSpan(5.f); //Destroy actor after 3s of being dead
}

