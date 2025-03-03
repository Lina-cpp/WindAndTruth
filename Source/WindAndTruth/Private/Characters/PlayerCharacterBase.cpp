// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/PlayerCharacterBase.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

// Sets default values
APlayerCharacterBase::APlayerCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
		CameraBoom->SetupAttachment(GetRootComponent());
		CameraBoom->TargetArmLength = 300.0f;
	ViewCamera = CreateDefaultSubobject<UCameraComponent>("ViewCamera");
		ViewCamera->SetupAttachment(CameraBoom);
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
	
}

void APlayerCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APlayerCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveForward, ETriggerEvent::Triggered, this, &APlayerCharacterBase::Move);
	}
}

void APlayerCharacterBase::Move(const FInputActionValue& Value)
{
	const FVector2D Direction = Value.Get<FVector2D>();
	if (Controller)
	{
		FVector Forward = GetActorForwardVector();
		FVector Right = GetActorRightVector();
		AddMovementInput(GetActorForwardVector(), Direction.Y);
		AddMovementInput(GetActorRightVector(), Direction.X);
		//UE_LOG(LogTemp, Warning, TEXT("Move activ"));
		FString Message = FString::Printf(TEXT("Move Forward: %f  Move Right: %f"), Direction.Y, Direction.X );
		GEngine->AddOnScreenDebugMessage(3, 1, FColor::Green, Message);
	}
}
