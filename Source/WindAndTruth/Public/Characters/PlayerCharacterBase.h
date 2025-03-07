// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Pawns/PosesPawns.h"
#include "CharacterTypes.h"

#include "PlayerCharacterBase.generated.h"

//Forward Declaring
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UGroomComponent;
class AItem;
class UAnimMontage;


UCLASS()
class WINDANDTRUTH_API APlayerCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	APlayerCharacterBase();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;

/**
*	InputMappings and it's functions
**/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
		UInputMappingContext* PlayerContextMapping;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
		UInputAction* MoveForward;
			void Move(const FInputActionValue& Value);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
		UInputAction* LookAround;
			void Look(const FInputActionValue& Value);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
		UInputAction* JumpAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
		UInputAction* InteractionAction;
			void Interaction(const FInputActionValue& Value);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
		UInputAction* AttackAction;
			void Attack(const FInputActionValue& Value);


	
/**
*	Play montage functions
**/
	void PlayAttackMontage();
	UFUNCTION(BlueprintCallable)
	void AttackEnd();
	bool CanAttack();


	
private:
/**
* Character Components
**/	
	//Grooms
	UPROPERTY(VisibleAnywhere, Category = Hair)
		UGroomComponent* Hair;
	UPROPERTY(VisibleAnywhere, Category = Hair)
		UGroomComponent* Eyebrows;
		
	UPROPERTY(VisibleAnywhere)
		USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere)
		UCameraComponent* ViewCamera;
	UPROPERTY(VisibleInstanceOnly)
		AItem* OverlappingItem; //Ref to an item we are overlapping right now

	
/**
* ENUMS
**/
	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	EActionState ActionState = EActionState::EAS_Unoccupied;


	
/**
* Animation Montages
**/
	UPROPERTY(EditDefaultsOnly, Category = Montages)
		UAnimMontage* AttackMontage;



	
public:
	FORCEINLINE void SetOverlappingItem(AItem* Item) { OverlappingItem = Item; }
	//public getter for EnumVar so PlayerChar_AnimInstance can get the value
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }

};

