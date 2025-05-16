// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/PlayerCharacterBase.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Items/Item.h"
#include "Items/Weapons/Weapon.h"
#include "Animation/AnimMontage.h"
#include "GroomComponent.h"
#include "AssetTypeActions/AssetDefinition_SoundBase.h"
#include "Components/AttributeComponent.h"
#include "Components/StaticMeshComponent.h"
#include "HUD/PlayerHUD.h"
#include "Items/Soul.h"
#include "Items/Treasure.h"
#include "HUD/PlayerOverlay.h"
#include "HUD/PlayerHUD.h"

// Sets default values
APlayerCharacterBase::APlayerCharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;
	//roation off so character won't be 'glued' to the screen and rotate with camera
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true; //Character will turn to directions he is moving
	GetCharacterMovement()->RotationRate = FRotator(0.f, 500.f, 0.f); //Rotation speed

	GetMesh()->SetCollisionObjectType(ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);
	
	//Spring arm and Camera
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
		CameraBoom->SetupAttachment(GetRootComponent());
		CameraBoom->TargetArmLength = 300.0f;
	ViewCamera = CreateDefaultSubobject<UCameraComponent>("ViewCamera");
		ViewCamera->SetupAttachment(CameraBoom);

	//Grooms
	Hair = CreateDefaultSubobject<UGroomComponent>("Hair");
		Hair->SetupAttachment(GetMesh());
		Hair->AttachmentName = FString("head");
	
	Eyebrows = CreateDefaultSubobject<UGroomComponent>("Eyebrows");
		Eyebrows->SetupAttachment(GetMesh());
		Eyebrows->AttachmentName = FString("head");
}

void APlayerCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
	//IMC - getting subsystem and ading IMC
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(PlayerContextMapping, 0);
		}
	}

	Tags.Add(FName("EngageableTarget")); //Tag - First use in Enemy.cpp to check is SeenPawn is Player
	InitializePlayerOverlay();
	
}

void APlayerCharacterBase::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	Super::GetHit_Implementation(ImpactPoint, Hitter);

	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
	ActionState = EActionState::EAS_HitReaction;
}

float APlayerCharacterBase::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	class AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage(DamageAmount);
	return DamageAmount;
}

void APlayerCharacterBase::SetOverlappingItem(AItem* Item)
{
	OverlappingItem = Item;
}

void APlayerCharacterBase::AddSouls(ASoul* Soul)
{
	if (Attributes && PlayerOverlay)
	{
		Attributes->AddSouls(Soul->GetSouls());
		PlayerOverlay->SetSouls(Soul->GetSouls());
	}
}

void APlayerCharacterBase::AddGold(ATreasure* Treasure)
{
	if (Attributes&& PlayerOverlay)
	{
		Attributes->AddGold(Treasure->GetGold());
		PlayerOverlay->SetGold(Attributes->GetGold());
	}
}


/**
* Input Actions & IMC
**/
void APlayerCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveForward, ETriggerEvent::Triggered, this, &APlayerCharacterBase::Move);
		EnhancedInputComponent->BindAction(LookAround, ETriggerEvent::Triggered, this, &APlayerCharacterBase::Look);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(InteractionAction, ETriggerEvent::Started, this, &APlayerCharacterBase::Interaction);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &APlayerCharacterBase::Attack);
		EnhancedInputComponent->BindAction(WeaponEquip, ETriggerEvent::Started, this, &APlayerCharacterBase::EquipWeaponFromBack);
		
	}
}

void APlayerCharacterBase::Move(const FInputActionValue& Value)
{
	if (ActionState == EActionState::EAS_Attacking) return; //if in atacking/equiping weapon state leave this function
	const FVector2D Direction = Value.Get<FVector2D>();
	if (Controller)
	{
		const FRotator ControlRotation = GetControlRotation();
		const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f); //only Yaw matters (turning left/right)

		
		//With this code, PlayerCharacter will move to direction that camera "points"
		//Works only with Forward/Backwards
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X); //UnitVector (length 1) of Yaw (MatrixRotation calculate where ForwardVector is)
		AddMovementInput(ForwardDirection, Direction.Y);

		//Same thing but Right/Left
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(RightDirection, Direction.X);
	}
}

inline void APlayerCharacterBase::Look(const FInputActionValue& Value)
{
	const FVector2D Axis = Value.Get<FVector2D>();
	if (Controller)
	{
		AddControllerYawInput(Axis.X);
		AddControllerPitchInput(Axis.Y);
	}
}

void APlayerCharacterBase::Interaction(const FInputActionValue& Value)
{
	AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);
	if (OverlappingWeapon)
	{
		EquipWeapon(OverlappingWeapon);
	}
	else GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Purple, FString("WeaponNotValid"));
}

void APlayerCharacterBase::AttackInput(const FInputActionValue& Value)
{
	Attack();
}


/**
*	Weapon Functions
**/

void APlayerCharacterBase::EquipWeaponFromBack(const FInputActionValue& Value)
{
	if (CanDisarm())
	{
		Disarm();
	}
	else if (CanArm())
	{
		Arm();
	}
	
}

void APlayerCharacterBase::EquipWeapon(AWeapon* Weapon)
{
	Weapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
	CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
	OverlappingItem = nullptr;
	EquippedWeapon = Weapon;
}


void APlayerCharacterBase::AttachWeaponToBack()
{
	if (EquippedWeapon)
	{
		//Call Function from weapon to attach weapon mesh to socket on the back - added in ABP_Echo
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("SpineSocket"));
	}
}

void APlayerCharacterBase::AttachWeaponToHand()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("RightHandSocket"));
	}
}

void APlayerCharacterBase::HitReactEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}


void APlayerCharacterBase::Attack()
{
	if (CanAttack()) //call bool function can attack
	{
		PlayAttackMontage();
		ActionState = EActionState::EAS_Attacking;
	}
}

void APlayerCharacterBase::AttackEnd()
{
	ActionState = EActionState::EAS_Unoccupied; //Function called in ABP_Echo on notify
}


/*
void APlayerCharacterBase::FinishEquipping()
{
	ActionState = EActionState::EAS_Unoccupied;
}
*/


/*
 *	Anim Montages Functions
 */


void APlayerCharacterBase::PlayEquipMontage(const FName SectionName)
{

	//Get instance and play montage by section (Section is selected inEquipWeaponFromBack())
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && EquipMontage)
	{
		AnimInstance->Montage_Play(EquipMontage);
		AnimInstance->Montage_JumpToSection(SectionName, EquipMontage);
	}
	
}

/*	Hud Stuff	*/

void APlayerCharacterBase::InitializePlayerOverlay()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		APlayerHUD* PlayerHUD = Cast<APlayerHUD>(PlayerController->GetHUD());
		if (PlayerHUD)
		{
			PlayerOverlay = PlayerHUD->GetPlayerOverlay();
			if (PlayerOverlay && Attributes)
			{
				PlayerOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
				PlayerOverlay->SetStaminaBarPercent(1.f);
				PlayerOverlay->SetGold(0);
				PlayerOverlay->SetSouls(0);
			}
		}
	}
}




/**
 * Bool Functions
 **/
bool APlayerCharacterBase::CanAttack()
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState != ECharacterState::ECS_Unequipped;
}

bool APlayerCharacterBase::CanDisarm()
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState != ECharacterState::ECS_Unequipped;
}

bool APlayerCharacterBase::CanArm()
{
	return ActionState == EActionState::EAS_Unoccupied &&
	CharacterState == ECharacterState::ECS_Unequipped &&
	EquippedWeapon;
}

void APlayerCharacterBase::Disarm()
{
	PlayEquipMontage(FName("Unequip"));
	CharacterState = ECharacterState::ECS_Unequipped;
	//ActionState = EActionState::EAS_EquippingWeapon;
}

void APlayerCharacterBase::Arm()
{
	PlayEquipMontage(FName("Equip"));
	CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
	//ActionState = EActionState::EAS_EquippingWeapon;
}


