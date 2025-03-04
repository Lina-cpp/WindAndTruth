// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/PlayerChar_AnimInstance.h"
#include "Characters/PlayerCharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UPlayerChar_AnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Character = Cast<APlayerCharacterBase>(TryGetPawnOwner()); //cast to playerchar and set it
	if (Character)
	{
		CharacterMovementComponent = Character->GetCharacterMovement(); //if cast succeed set MovementComponent
	}
}

void UPlayerChar_AnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (CharacterMovementComponent)
	{
		//Get Function from KismetMathLib to GetVectorLength of XY
		GroundSpeed = UKismetMathLibrary::VSizeXY(CharacterMovementComponent->Velocity);
	}
}
