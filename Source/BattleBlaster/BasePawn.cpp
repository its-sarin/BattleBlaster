// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePawn.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABasePawn::ABasePawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Component"));
	SetRootComponent(CapsuleComp);

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base Mesh"));
	BaseMesh->SetupAttachment(CapsuleComp);

	TurretMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Turret Mesh"));
	TurretMesh->SetupAttachment(BaseMesh);

	ProjectileSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Projectile Spawn Point"));
	ProjectileSpawnPoint->SetupAttachment(TurretMesh);
}

void ABasePawn::RotateTurret(FVector LookAtTarget)
{
	FVector ToTarget = LookAtTarget - TurretMesh->GetComponentLocation();
	FRotator LookAtRotation = FRotator(0.f, ToTarget.Rotation().Yaw, 0.f);

	SetTurretRotation(LookAtRotation);
}

void ABasePawn::SetTurretRotation(FRotator TargetRotation)
{
	if (bUseSmoothTurretRotation)
	{
		FRotator SmoothedRotation = FMath::RInterpTo(
			TurretMesh->GetComponentRotation(),
			TargetRotation,
			GetWorld()->GetDeltaSeconds(),
			SmoothTurretRotationSpeed
		);
		TurretMesh->SetWorldRotation(SmoothedRotation);
		return;
	}

	TurretMesh->SetWorldRotation(TargetRotation);
}

void ABasePawn::Fire()
{
	FVector SpawnLocation = ProjectileSpawnPoint->GetComponentLocation();
	FRotator SpawnRotation = ProjectileSpawnPoint->GetComponentRotation();
	
	AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, SpawnLocation, SpawnRotation);
	if (Projectile)
	{
		Projectile->SetOwner(this);
		AActor* ProjectileOwner = Projectile->GetOwner();
		if (ProjectileOwner)
		{
			UE_LOG(LogTemp, Warning, TEXT("Projectile fired by: %s"), *ProjectileOwner->GetName());
		}
	}
}

void ABasePawn::HandleDestruction()
{
	if (DeathEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			DeathEffect,
			GetActorLocation(),
			GetActorRotation()
		);
	}

	if (DeathSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			DeathSound,
			GetActorLocation(),
			1.f,
			0.75f
		);
	}
	if (DeathCameraShakeClass)
	{
		if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0))
		{
			PlayerController->ClientStartCameraShake(DeathCameraShakeClass);
		}
	}
}

