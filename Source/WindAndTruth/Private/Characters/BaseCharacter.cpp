// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/BaseCharacter.h"
#include "Items/Weapons/Weapon.h" //weapon
#include "Components/BoxComponent.h" //for weapon box
#include "Components/AttributeComponent.h" //Components with Attributes hp, etc.


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

void ABaseCharacter::AttackEnd()
{
	
}

void ABaseCharacter::Die()
{
	
}


/**
*	Play montage functions
**/

void ABaseCharacter::PlayAttackMontage()
{
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
