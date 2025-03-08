// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawns/PosesPawns.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"


APosesPawns::APosesPawns()
{
	PrimaryActorTick.bCanEverTick = true;
	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
		SetRootComponent(Capsule);
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
		SkeletalMesh->SetupAttachment(Capsule);
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
		SpringArm->SetupAttachment(Capsule);
		SpringArm->TargetArmLength = 300.f;
	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
		ViewCamera->SetupAttachment(SpringArm);
	
	//AutoPossessPlayer = EAutoReceiveInput::Player0;
	
}

void APosesPawns::BeginPlay()
{
	Super::BeginPlay();

	//IMC in C++ - Cast to player controller and get controller
	// Doing this in If makes it valid only in If so, after doing this, we have one less pointer #optimization
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		//Creating pointer to Subsystem then get Subsytem from LocalPlayer and pass LocalPlayer than we get from PlayerController
		//Just like in BP we get EnhancedInputSubsystem
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			//Add mapping context and priority
			Subsystem->AddMappingContext(PawnMappingContext,0);
		}
	}
	
}

void APosesPawns::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}


//
//	IMC
//
// Called to bind functionality to input
void APosesPawns::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	//cast from PlayerInputComponent to it's enhanced version so we can bind actions
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APosesPawns::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APosesPawns::Look);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &APosesPawns::Interact);
	}
}

// IMC
void APosesPawns::Move(const FInputActionValue& Value)
{
	const FVector2D DirectionValue = Value.Get<FVector2D>();
	if (GetController())
	{
		FVector Forward = GetActorForwardVector();
		AddMovementInput(Forward, DirectionValue.Y);
	}
}

void APosesPawns::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisValue = Value.Get<FVector2D>(); //save value (Y & X)
	if (GetController())
	{
		AddControllerYawInput(LookAxisValue.X);
		AddControllerPitchInput(LookAxisValue.Y);
	}
}

void APosesPawns::Interact(const FInputActionValue& Value)
{
	const bool Input = Value.Get<bool>();
	if (GetController())
	{
		GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, "Interacting");
	}
}
