// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/HealthBarComponent.h"

#include "Components/ProgressBar.h"
#include "HUD/HealthBar.h"
#include "Components/ProgressBar.h"

void UHealthBarComponent::SetHealthPercent(float Percent)
{
	//If Widget is null, cast to healthbar, then ChangeHealthBar Percentage
	if (HealthBarWidget == nullptr)
	{
		HealthBarWidget = Cast<UHealthBar>(GetUserWidgetObject()); //cast to healthbar
	}
	
	if (HealthBarWidget && HealthBarWidget->HealthBar)
	{
		HealthBarWidget->HealthBar->SetPercent(Percent);
	}
}
