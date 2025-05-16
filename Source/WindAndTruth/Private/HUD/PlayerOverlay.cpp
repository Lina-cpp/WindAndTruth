// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/PlayerOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UPlayerOverlay::SetHealthBarPercent(float Percent)
{
	if (HealthProgressBar)
	{
		HealthProgressBar->SetPercent(Percent);
	}
}

void UPlayerOverlay::SetStaminaBarPercent(float Percent)
{
	if (StaminaProgressBar)
	{
		StaminaProgressBar->SetPercent(Percent);
	}
}

void UPlayerOverlay::SetGold(int32 Gold)
{
	if (GoldCount)
	{
		//Convert int32 to text
		const FString String = FString::Printf(TEXT("%d"), Gold);
		const FText Text = FText::FromString(String);
		GoldCount->SetText(Text);
	}
}

void UPlayerOverlay::SetSouls(int32 Souls)
{
	if (SoulsCount)
	{
		GoldCount->SetText(FText::FromString(FString::Printf(TEXT("%d"), Souls)));
	}
}
