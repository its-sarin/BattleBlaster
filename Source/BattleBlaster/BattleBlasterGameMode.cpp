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
		}
	}
	
	// Setup Player HUD
	if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		ScreenMessageWidget = CreateWidget<UScreenMessage>(PlayerController, ScreenMessageClass);
		if (ScreenMessageWidget)
		{
			ScreenMessageWidget->AddToPlayerScreen();
			ScreenMessageWidget->SetMessageText("Get Ready!");
		}
		PlayerHUDWidget = CreateWidget<UPlayerHUD>(PlayerController, PlayerHUDClass);
		if (PlayerHUDWidget)
		{
			PlayerHUDWidget->AddToPlayerScreen();
			PlayerHUDWidget->SetTowersRemainingText(TowerCount);
		}
	}

	// Countdown to start the game
	GameStartCountdownSeconds = GameStartCountdown;
	GetWorldTimerManager().SetTimer(GameStartTimerHandle, this, &ABattleBlasterGameMode::OnCountdownTimerTimeout, 1.f, true);
}

void ABattleBlasterGameMode::OnCountdownTimerTimeout()
{
	GameStartCountdownSeconds--;
	
	if (GameStartCountdownSeconds > 0)
	{
		ScreenMessageWidget->SetMessageText(FString::FromInt(GameStartCountdownSeconds));
	}
	else if (GameStartCountdownSeconds == 0)
	{
		ScreenMessageWidget->SetMessageText("Go!");
		if (Tank)
		{
			Tank->SetPlayerEnabled(true);
		}
	}
	else
	{
		GetWorldTimerManager().ClearTimer(GameStartTimerHandle);
		ScreenMessageWidget->SetVisibility(ESlateVisibility::Hidden);
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

			PlayerHUDWidget->SetTowersRemainingText(TowerCount);			

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

		ScreenMessageWidget->SetMessageText(GameOverString);
		ScreenMessageWidget->SetVisibility(ESlateVisibility::Visible);

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
