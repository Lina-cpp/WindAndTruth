// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

UCLASS()
class WINDANDTRUTH_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	AItem();		// Sets default values for this actor's properties
	virtual void Tick(float DeltaTime) override;
	
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sinus Settings")
	float Amplitude = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sinus Settings")
	float TimeConstant = 5.f;

	UFUNCTION(BlueprintPure, Category = "Sinus Settings")
	float TransformedSin();

	UFUNCTION(BlueprintPure, Category = "Sinus Settings")
	float TransformedCosin();

	template<typename T> //calling AVG function I need to specify typename for it can be float, double, int etc
	T Avg(T First, T Second);
	
private:	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sinus Settings", meta = (AllowPrivateAccess = "true"))
	float RunningTime;
	





};

template <typename T>
T AItem::Avg(T First, T Second)
{
	return  (First + Second) / 2;
}
