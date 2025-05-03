// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/BaseCharacter.h"
#include "Items/Weapons/Weapon.h" //weapon
#include "Components/BoxComponent.h" //for weapon box
#include "Components/AttributeComponent.h" //Components with Attributes hp, etc.
#include "Kismet/GameplayStatics.h" //SFX & VFX
#include "Components/CapsuleComponent.h" //Turn Off capsule collision


ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	Attributes = CreateDefaultSubobject<UAttributeComponent>("Attributes");

}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseCharacter::SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{
	if (EquippedWeapon && EquippedWeapon->GetWeaponBox())
	{
		EquippedWeapon->GetWeaponBox()->SetCollisionEnabled((CollisionEnabled)); //Enable/Disable Collision for weapon - ABP_Echo
		EquippedWeapon->IgnoreActors.Empty(); //Clear Actors array
	}
}


/*
*	Combat & Weapons
*/

bool ABaseCharacter::CanAttack()
{
	return false;
}

void ABaseCharacter::Attack()
{
	
}

bool ABaseCharacter::IsAlive()
{
	return Attributes && Attributes->IsAlive();
}

void ABaseCharacter::AttackEnd()
{
	
}

void ABaseCharacter::Die()
{
	
}

void ABaseCharacter::HandleDamage(float DamageAmount)
{
	if (Attributes)
	{
		Attributes->ReceiveDamage(DamageAmount); //Calculate damage based on input dmg and hp
	}
}

/**
*	SFX & VFX
**/

void ABaseCharacter::PlayHitSound(const FVector& ImpactPoint)
{
	if (HitSound) UGameplayStatics::PlaySoundAtLocation(this, HitSound,ImpactPoint); //if valid, play sound at location
}

void ABaseCharacter::SpawnHitParticle(const FVector& ImpactPoint)
{
	if (HitParticle) UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticle, ImpactPoint);
}

/**
*	Play montage functions
**/


void ABaseCharacter::PlayMontageSection(UAnimMontage* Montage, const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance(); //Get Mesh's AnimInstance
	if (AnimInstance && Montage)	//check if instance and Montage isn't null
	{
		AnimInstance->Montage_Play(Montage); //Play montage
		AnimInstance->Montage_JumpToSection(SectionName, Montage); //Jump to section of this montage
	}
}

int32 ABaseCharacter::PlayRandomMontageSection(UAnimMontage* Montage, const TArray<FName>& SectionNames)
{
	if (SectionNames.Num() <= 0) return -1;
	const int32 MaxSectionIndex = SectionNames.Num() - 1; //get how many sections there is
	const int32 Selection = FMath::RandRange(0, MaxSectionIndex); // get random number
	PlayMontageSection(Montage, SectionNames[Selection]);
	
	return Selection;
}


int32 ABaseCharacter::PlayAttackMontage()
{
	return PlayRandomMontageSection(AttackMontage, AttackMontageSections);
}

int32 ABaseCharacter::PlayDeathMontage()
{
	return PlayRandomMontageSection(DeathMontage, DeathMontageSections);
}

void ABaseCharacter::DisableCapsule()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision); //Disable Enemy Collision when is dead
}

void ABaseCharacter::PlayHitReactMontage(const FName SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();	//get anim instance
	if (AnimInstance && HitReactMontage) // check if AnimInstance is valid & AM is not null
	{
		AnimInstance->Montage_Play(HitReactMontage); //Play Montage
		AnimInstance->Montage_JumpToSection(SectionName, HitReactMontage); //Jump to selected section
	}
}

void ABaseCharacter::DirectionalHitReact(const FVector& ImpactPoint)
{
	const FVector Forward = GetActorForwardVector(); // get enemy's Forward Vector (it's already normalized)
	const FVector ImpactLowered(ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z); //debug for arrows
	const FVector ToHit = (ImpactLowered - GetActorLocation()).GetSafeNormal(); //Vector location of ImpactPoint and ActorLocation (for DotProduct to calculate which anim to play)
	// .GetSafeNormal to normalize vector
	// Forward * ToHit = |Forward||ToHit| * cos(theta)
	// |Forward| = 1, |ToHit| = 1, so Forward * ToHit = cos(theta)
	const double CosTheta = FVector::DotProduct(Forward, ToHit); //save DotProduct to Var
	//Take the inverse cosine (arc-cosine) of cos(theta) to get theta
	double Theta = FMath::Acos(CosTheta); //Angle in radians
	Theta = FMath::RadiansToDegrees(Theta); //Convert radians to Degrees
	
	//if crossprduct points down, Theta should be negative
	const FVector CrossProduct = FVector::CrossProduct(Forward, ToHit);
	if (CrossProduct.Z < 0)
	{
		Theta *= -1.f;
	}

	FName Section("FromBack"); //Default Value of section, then we check theta
	if (Theta >= -45.f && Theta < 45.f) Section = FName("FromFront");
	else if (Theta >= -135.f && Theta < -45.f) Section = FName("FromLeft");
	else if (Theta >= 45.f && Theta < 135.f) Section = FName("FromRight");
	
	PlayHitReactMontage(Section);

}
