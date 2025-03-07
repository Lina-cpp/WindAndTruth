// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CharacterTypes.h"

#include "PlayerChar_AnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class WINDANDTRUTH_API UPlayerChar_AnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	//cpp vesrion of "Event Blueprint Initialize Animation" aka BeginPlay
	virtual void NativeInitializeAnimation() override;
	//cpp version of "Event Blueprint Update Animation" aka Tick
	virtual void NativeUpdateAnimation(float DeltaTime) override;

	UPROPERTY(BLueprintReadOnly) //char pointer that we will cast to
		class APlayerCharacterBase* PlayerCharacter;
	UPROPERTY(BlueprintReadOnly, Category = Movement) //movement var for ground speed, jumping etc.
		class UCharacterMovementComponent* CharacterMovementComponent;
	UPROPERTY(BlueprintReadOnly, Category = Movement)
		float GroundSpeed;
	UPROPERTY(BlueprintReadOnly, Category = Movement)
		bool IsFalling;
	UPROPERTY(BlueprintReadOnly, Category = "Movement | Character State")
		ECharacterState CharacterState; //Enum with current player State - Changed in PlayerCharacterBase
};
