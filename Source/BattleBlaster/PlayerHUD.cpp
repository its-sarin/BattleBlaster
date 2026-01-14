// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHUD.h"
#include "Components/ProgressBar.h"
#include "HealthComponent.h"

void UPlayerHUD::OnHealthChanged(float NewHealth, float MaxHealth)
{
	float Percentage = NewHealth / MaxHealth;
	HealthBar->SetPercent(Percentage);
}

void UPlayerHUD::SetTowersRemainingText(int32 TowersRemaining)
{
	if (TowersRemainingTextBlock)
	{
		TowersRemainingTextBlock->SetText(FText::AsNumber(TowersRemaining));
	}
}

void UPlayerHUD::SetTargetHealthComponent(UHealthComponent* NewHealthComponent)
{
	// Early exit if already set to the same component
	if (TargetHealthComponent == NewHealthComponent)
	{
		return;
	}

	//Unregister from the old target health component
	if (TargetHealthComponent.IsValid())
	{
		TargetHealthComponent->OnHealthChanged.RemoveDynamic(this, &UPlayerHUD::OnHealthChanged);
	}

	TargetHealthComponent = NewHealthComponent;

	//Register to the new target health component
	if (TargetHealthComponent.IsValid())
	{
		TargetHealthComponent->OnHealthChanged.AddDynamic(this, &UPlayerHUD::OnHealthChanged);
	}
}
