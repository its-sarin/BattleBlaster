// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "PlayerHUD.generated.h"

class UProgressBar;
class UHealthComponent;

/**
 * 
 */
UCLASS()
class BATTLEBLASTER_API UPlayerHUD : public UUserWidget
{
	GENERATED_BODY()
	TWeakObjectPtr<UHealthComponent> TargetHealthComponent;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* TowersRemainingTextBlock;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UProgressBar* HealthBar;

	UFUNCTION(BlueprintCallable)
	void SetTargetHealthComponent(UHealthComponent* NewHealthComponent);

	UFUNCTION()
	void OnHealthChanged(float NewHealth, float MaxHealth);

	void SetTowersRemainingText(int32 TowersRemaining);
};
