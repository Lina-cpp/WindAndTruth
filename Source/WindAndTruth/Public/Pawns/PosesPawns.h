// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"

#include "PosesPawns.generated.h"

class UCapsuleComponent; //forward declaration of using UCapsuleComponent (.h added in .cpp)
class USpringArmComponent;
class UCameraComponent;

UCLASS()
class WINDANDTRUTH_API APosesPawns : public APawn
{
	GENERATED_BODY()

public:
	APosesPawns();
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
protected:
	virtual void BeginPlay() override;

private:
		UPROPERTY(VisibleAnywhere)
			UCapsuleComponent* Capsule;
		UPROPERTY(VisibleAnywhere)
			USkeletalMeshComponent* SkeletalMesh;
		UPROPERTY(visibleAnywhere)
			USpringArmComponent* SpringArm;
		UPROPERTY(visibleAnywhere)
			UCameraComponent* ViewCamera;

};
