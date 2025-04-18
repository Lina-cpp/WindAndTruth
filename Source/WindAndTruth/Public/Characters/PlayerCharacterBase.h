// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pawns/PosesPawns.h"
#include "CharacterTypes.h"
#include "BaseCharacter.h"

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
class WINDANDTRUTH_API APlayerCharacterBase : public ABaseCharacter
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
			void AttackInput(const FInputActionValue& Value);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
		UInputAction* WeaponEquip;
			void EquipWeaponFromBack(const FInputActionValue& Value);


	
/**
*	Play montage functions
**/
	virtual void PlayAttackMontage() override;
	virtual void AttackEnd() override;
	void PlayEquipMontage(const FName SectionName);

	
/**
*	Weapon Functions
**/
	
	virtual bool CanAttack() override;
	bool CanDisarm();
	bool CanArm();
	virtual void Attack() override;
	


/**
*	Weapon Anim Notifies
**/
	UFUNCTION(BlueprintCallable)
	void Disarm(); // Attaches weapon to the back
	UFUNCTION(BlueprintCallable)
	void ArmWeapon();

	//UFUNCTION(BlueprintCallable) //in case if I'd like to force stop character while equiping weapon
	//void FinishEquipping();

	
private:
/**
* Character Components
**/	
	//Grooms
	UPROPERTY(VisibleAnywhere, Category = Hair)
		UGroomComponent* Hair;
	UPROPERTY(VisibleAnywhere, Category = Hair)
		UGroomComponent* Eyebrows;
	//Components
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
		UAnimMontage* EquipMontage;

	
public:
	FORCEINLINE void SetOverlappingItem(AItem* Item) { OverlappingItem = Item; }
	//public getter for EnumVar so PlayerChar_AnimInstance can get the value
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }

};

