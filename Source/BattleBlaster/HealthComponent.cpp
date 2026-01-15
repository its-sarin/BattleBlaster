// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;

	GetOwner()->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::OnDamageTaken);

	AGameModeBase* GameMode = UGameplayStatics::GetGameMode(GetWorld());
	if (GameMode)
	{
		BattleBlasterGameMode = Cast<ABattleBlasterGameMode>(GameMode);
	}

	if (ATank* Tank = Cast<ATank>(GetOwner()))
	{
		PlayerHUDWidget = BattleBlasterGameMode->PlayerHUDWidget;
		PlayerHUDWidget->SetTargetHealthComponent(this);
	}
}

void UHealthComponent::OnDamageTaken(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0.f)
	{
		return;
	}
	CurrentHealth -= Damage;

	// If DamagedActor is The Player, update HUD
	if (PlayerHUDWidget)
	{
		OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
	}

	if (CurrentHealth <= 0.f)
	{
		CurrentHealth = 0.f;

		if (BattleBlasterGameMode)
		{
			BattleBlasterGameMode->ActorDied(DamagedActor);
		}
	}
}

