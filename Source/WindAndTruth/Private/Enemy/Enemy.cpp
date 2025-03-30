// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "WindAndTruth/DebugMacros.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AttributeComponent.h"
#include "HUD/HealthBarComponent.h"


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
	Attributes = CreateDefaultSubobject<UAttributeComponent>("Attributes");
	HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>("Health Bar");
		HealthBarWidget->SetupAttachment(GetRootComponent());
	
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	//Check HP and set bar correctly
	if (Attributes && HealthBarWidget)
	{
		HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
	}
	
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}




/**
 *	Functions
**/

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint)
{
	DirectionalHitReact(ImpactPoint);
	if (HitSound) UGameplayStatics::PlaySoundAtLocation(this, HitSound,ImpactPoint); //if valid, play sound at location
	if (HitParticle) UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticle, ImpactPoint);
}


void AEnemy::DirectionalHitReact(const FVector& ImpactPoint)
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
	
	/*
* Debug

	UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + CrossProduct * 100, 15.f, FColor::Blue, 5.f );
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(1, 5, FColor::Red, FString::Printf(TEXT("Theta: %f"), Theta));
	}
	UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation()+Forward*60.f, 15.f, FColor::Red, 5.f );
	UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + ToHit * 60.f, 15.f, FColor::Green, 5.f);
	*/	
}

float AEnemy::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	class AController* EventInstigator, AActor* DamageCauser)
{
	if (Attributes && HealthBarWidget) //check if attributes is valid (component with health)
	{
		Attributes->ReceiveDamage(DamageAmount); //Calculate damage based on input dmg and hp
		HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent()); //update widget with matching %
	}
	return DamageAmount;
}








/*
 * Montages
*/
void AEnemy::PlayHitReactMontage(const FName SectionName)
{
	
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();	//get anim instance
	if (AnimInstance && HitReactMontage) // check if AnimInstance is valid & AM is not null
	{
		AnimInstance->Montage_Play(HitReactMontage); //Play Montage
		AnimInstance->Montage_JumpToSection(SectionName, HitReactMontage); //Jump to selected section
	}
}
