// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthBar.generated.h"



UCLASS()
class WINDANDTRUTH_API UHealthBar : public UUserWidget
{
	GENERATED_BODY()

public:
	//Var name MUST match name in editor - so HealthBar must match "HealthBar" in WBP_HealthBar
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthBar;
};
