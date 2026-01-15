// Fill out your copyright notice in the Description page of Project Settings.


#include "AmmoWidget.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"

void UAmmoWidget::SetAmmoCountText(int32 AmmoCount)
{
	AmmoCountTextBlock->SetText(FText::AsNumber(AmmoCount));
}

void UAmmoWidget::SetReloadProgress(float Progress)
{
	ReloadProgressBar->SetPercent(Progress);
}