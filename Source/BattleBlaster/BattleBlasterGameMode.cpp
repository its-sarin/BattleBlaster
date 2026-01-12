// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleBlasterGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Tower.h"

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
