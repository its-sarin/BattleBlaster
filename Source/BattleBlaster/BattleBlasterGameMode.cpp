// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleBlasterGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Tower.h"
#include "BattleBlasterGameInstance.h"

void ABattleBlasterGameMode::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> Towers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATower::StaticClass(), Towers);
	TowerCount = Towers.Num();
	UE_LOG(LogTemp, Warning, TEXT("Tower Count: %d"), TowerCount);

	Tank = Cast<ATank>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (!Tank)
	{
		UE_LOG(LogTemp, Error, TEXT("GameMode: Failed to find the Tank actor!"));
	}

	for (AActor* TowerActor : Towers)
	{
		ATower* Tower = Cast<ATower>(TowerActor);
		if (Tower && Tank)
		{
			Tower->Tank = Tank;
			UE_LOG(LogTemp, Warning, TEXT("GameMode: Assigned Tank to Tower %s"), *Tower->GetName());
		}
	}
}

void ABattleBlasterGameMode::ActorDied(AActor* DeadActor)
{
	bool bIsGamerOver = false;

	if (DeadActor == Tank)
	{
		Tank->HandleDestruction();
		
		bIsGamerOver = true;
	}
	else
	{
		ATower* DeadTower = Cast<ATower>(DeadActor);
		if (DeadTower)
		{
			DeadTower->HandleDestruction();

			TowerCount--;

			UE_LOG(LogTemp, Warning, TEXT("Tower Destroyed! Remaining Towers: %d"), TowerCount);
			if (TowerCount <= 0)
			{				
				bIsGamerOver = true;
				bIsVictory = true;
			}
		}
	}

	if (bIsGamerOver)
	{
		FString GameOverString = bIsVictory ? TEXT("Victory!") : TEXT("Defeat!");

		UE_LOG(LogTemp, Warning, TEXT("Game Over: %s"), *GameOverString);

		FTimerHandle GameOverTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(GameOverTimerHandle, this, &ABattleBlasterGameMode::OnGameOverTimerTimeout, GameOverDelay, false);
		UE_LOG(LogTemp, Warning, TEXT("Restarting Level..."));
	}
}

void ABattleBlasterGameMode::OnGameOverTimerTimeout()
{	
	if (UBattleBlasterGameInstance* BBGameInstance = Cast<UBattleBlasterGameInstance>(GetGameInstance()))
	{
		if (bIsVictory)
		{
			// Load next level
			BBGameInstance->LoadNextLevel();
		}
		else
		{
			// Reload current level
			BBGameInstance->RestartCurrentLevel();
		}
	}

	
}
