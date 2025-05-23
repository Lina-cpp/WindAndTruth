// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/HitInterface.h"
#include "BreakableActor.generated.h"

struct FChaosBreakEvent;
class UGeometryCollectionComponent;

UCLASS()
class WINDANDTRUTH_API ABreakableActor : public AActor, public IHitInterface
{
	GENERATED_BODY()
	
public:	
	ABreakableActor();
	virtual void Tick(float DeltaTime) override;

	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override; //Interafce Ovverride

	
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UCapsuleComponent* Capsule;

	UFUNCTION()
	void OnBreakEvent(const FChaosBreakEvent& BreakEvent);

private:
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = true))
	UGeometryCollectionComponent* GeometryCollection;

	//Array of treasures + wrapper + forward declaring ATreasure
	UPROPERTY(EditAnywhere, Category= "Breakable Properties")
	TArray<TSubclassOf<class ATreasure>> TreasureClasses;

	bool bBroken = false;
};
