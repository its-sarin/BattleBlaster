// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "Tank.h"
#include "ScreenMessage.h"
#include "PlayerHUD.h"

#include "BattleBlasterGameMode.generated.h"

/**
 * 
 */
UCLASS()
class BATTLEBLASTER_API ABattleBlasterGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UScreenMessage> ScreenMessageClass;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UPlayerHUD> PlayerHUDClass;

	UScreenMessage* ScreenMessageWidget;
	UPlayerHUD* PlayerHUDWidget;

	UPROPERTY(EditAnywhere, Category = "Game Start")
	int32 GameStartCountdown = 3;

	int32 GameStartCountdownSeconds;

	FTimerHandle GameStartTimerHandle;

	UPROPERTY(EditAnywhere, Category="Game Over")
	float GameOverDelay = 3.f;

	UPROPERTY(EditAnywhere, Category = "Game Over")
	bool bIsVictory = false;

	ATank* Tank;
	int32 TowerCount;

	void ActorDied(AActor* DeadActor);
	void OnGameOverTimerTimeout();
	void OnCountdownTimerTimeout();
};
