// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerOverlay.generated.h"

/**
 * 
 */
UCLASS()
class WINDANDTRUTH_API UPlayerOverlay : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetHealthBarPercent(float Percent);
	void SetStaminaBarPercent(float Percent);
	void SetGold(int32 Gold);
	void SetSouls(int32 Souls);
	
private:
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthProgressBar;
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* StaminaProgressBar;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* GoldCount;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* SoulsCount;
	
};

