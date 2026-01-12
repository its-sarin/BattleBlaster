// Fill out your copyright notice in the Description page of Project Settings.


#include "Tower.h"

void ATower::BeginPlay()
{
	Super::BeginPlay();

	FTimerHandle FireRateTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(FireRateTimerHandle, this, &ATower::CheckFireCondition, FireRate, true);
}

void ATower::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	TrackTank();
}

bool ATower::IsInFireRange()
{
	if (!Tank)
	{
		return false;
	}

	return (FVector::Dist(GetActorLocation(), Tank->GetActorLocation()) <= FireRange);
}

void ATower::TrackTank()
{
	if (IsInFireRange())
	{
		RotateTurret(Tank->GetActorLocation());
	}
}

void ATower::CheckFireCondition()
{
	if (IsInFireRange())
	{
		Fire();
	}
}
