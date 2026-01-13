// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include <Kismet/GameplayStatics.h>

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile Mesh"));
	SetRootComponent(ProjectileMesh);

	ProjectileMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement Component"));

	InitialLifeSpan = LifeSpan;
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	ProjectileMesh->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
		AActor* ProjectileOwner = GetOwner();
		if (ProjectileOwner)
		{
			if (OtherActor && OtherActor != ProjectileOwner && OtherActor != this)
			{
				UGameplayStatics::ApplyDamage(OtherActor,
					Damage,
					ProjectileOwner->GetInstigatorController(),
					this,
					UDamageType::StaticClass()
				);
			}
		}
		Destroy();
}

