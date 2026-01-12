// Fill out your copyright notice in the Description page of Project Settings.


#include "Tower.h"

void ATower::BeginPlay()
{
	Super::BeginPlay();

}

void ATower::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	TrackTank();
}

void ATower::TrackTank()
{
	if (Tank && FVector::Dist(GetActorLocation(), Tank->GetActorLocation()) <= FireRange)
	{
		RotateTurret(Tank->GetActorLocation());
	}
}
