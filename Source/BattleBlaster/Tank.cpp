// Fill out your copyright notice in the Description page of Project Settings.


#include "Tank.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "InputMappingContext.h"
#include "Kismet/KismetMathLibrary.h"
#include "BattleBlasterGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerHUD.h"

ATank::ATank()
{
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm Component"));
	SpringArmComp->SetupAttachment(CapsuleComp);

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera Component"));
	CameraComp->SetupAttachment(SpringArmComp);
}

// Called when the game starts or when spawned
void ATank::BeginPlay()
{
	Super::BeginPlay();

	PlayerController = Cast<APlayerController>(Controller);
	if (PlayerController)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	
	AGameModeBase* GameMode = UGameplayStatics::GetGameMode(GetWorld());
	if (GameMode)
	{
		BattleBlasterGameMode = Cast<ABattleBlasterGameMode>(GameMode);
		PlayerHUDWidget = BattleBlasterGameMode->PlayerHUDWidget;
	}

	CurrentAmmo = MaxAmmo;
	if (PlayerHUDWidget)
	{
		PlayerHUDWidget->SetAmmoCountText(CurrentAmmo);
	}

	SetPlayerEnabled(false);
}

// Called every frame
void ATank::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ReloadTimerHandle.IsValid())
	{
		float ElapsedTime = GetWorldTimerManager().GetTimerElapsed(ReloadTimerHandle);
		float ReloadProgress = (ElapsedTime / ReloadTime);

		if (PlayerHUDWidget)
		{
			PlayerHUDWidget->SetReloadProgress(ReloadProgress);
		}
	}

	// get the current rotation
	const FRotator OldRotation = TurretMesh->GetComponentRotation();

	// are we aiming with the mouse?
	if (bUsingMouse)
	{
		if (PlayerController)
		{
			// get the cursor world location
			FHitResult OutHit;
			PlayerController->GetHitResultUnderCursorByChannel(ETraceTypeQuery::TraceTypeQuery1, true, OutHit);

			// find the aim rotation 
			const FRotator AimRot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), OutHit.Location);

			// save the aim angle
			AimAngle = AimRot.Yaw;

			// update the yaw, reuse the pitch and roll
			SetTurretRotation(FRotator(OldRotation.Pitch, AimAngle, OldRotation.Roll));

		}
	}
	else
	{
		// use quaternion interpolation to blend between our current rotation
		// and the desired aim rotation using the shortest path
		const FRotator TargetRot = FRotator(OldRotation.Pitch, AimAngle, OldRotation.Roll);
		DrawDebugSphere(GetWorld(), TargetRot.Vector() * 500.f + GetActorLocation(), 25.f, 12, FColor::Red, false, -1.f, 0, 2.f);

		SetTurretRotation(TargetRot);
	}
}

// Called to bind functionality to input
void ATank::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATank::MoveInput);
		EnhancedInputComponent->BindAction(TurnAction, ETriggerEvent::Triggered, this, &ATank::TurnInput);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &ATank::DoFire);
		EnhancedInputComponent->BindAction(StickAimAction, ETriggerEvent::Triggered, this, &ATank::StickAim);
		EnhancedInputComponent->BindAction(MouseAimAction, ETriggerEvent::Triggered, this, &ATank::MouseAim);
		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started, this, &ATank::Reload);
	}
}

void ATank::MoveInput(const FInputActionValue& Value)
{
	float MoveValue = Value.Get<float>();

	FVector DeltaLocation = FVector::ZeroVector;
	DeltaLocation.X = MoveValue * Speed * GetWorld()->GetDeltaSeconds();

	AddActorLocalOffset(DeltaLocation, true);
}

void ATank::TurnInput(const FInputActionValue& Value)
{
	float TurnValue = Value.Get<float>();

	FRotator DeltaRotation = FRotator::ZeroRotator;
	DeltaRotation.Yaw = TurnValue * TurnRate * GetWorld()->GetDeltaSeconds();
	AddActorLocalRotation(DeltaRotation, true);
}

void ATank::StickAim(const FInputActionValue& Value)
{
	// lower the mouse controls flag
	bUsingMouse = false;

	// hide the mouse cursor
	if (PlayerController)
	{
		PlayerController->SetShowMouseCursor(false);
	}

	// get the input vector
	FVector2D InputVector = Value.Get<FVector2D>();

	// route the input
	DoAim(InputVector.X, InputVector.Y);
}

void ATank::MouseAim(const FInputActionValue & Value)
{
	// raise the mouse controls flag
	bUsingMouse = true;

	// show the mouse cursor
	if (PlayerController)
	{
		PlayerController->SetShowMouseCursor(true);
	}
}

void ATank::DoAim(float AxisX, float AxisY)
{
	// calculate the aim angle from the inputs
	AimAngle = FMath::RadiansToDegrees(FMath::Atan2(AxisY, -AxisX));

	// Adjust the aim angle relative to the tank's facing direction (this way, up on the stick is always forward for the tank)
	AimAngle += GetActorRotation().Yaw;
}

void ATank::DoFire()
{
	if (bIsReloading) {return;}

	if (CurrentAmmo > 0)
	{
		Fire();
		CurrentAmmo--;

		if (PlayerHUDWidget)
		{
			PlayerHUDWidget->SetAmmoCountText(CurrentAmmo);
		}
		
		UE_LOG(LogTemp, Warning, TEXT("Shot fired! Current Ammo: %d"), CurrentAmmo);

		// Start the reload timer if we have no ammo left
		if (CurrentAmmo <= 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("Reloading..."));
			
			Reload();
		}
	}
}

void ATank::Reload()
{
	if (bIsReloading || CurrentAmmo == MaxAmmo)
	{
		return;
	}

	bIsReloading = true;

	GetWorldTimerManager().SetTimer(ReloadTimerHandle, [this]()
	{
		CurrentAmmo = MaxAmmo;
		UE_LOG(LogTemp, Warning, TEXT("Reload complete! Current Ammo: %d"), CurrentAmmo);
		if (PlayerHUDWidget)
		{
			PlayerHUDWidget->SetAmmoCountText(CurrentAmmo);
			PlayerHUDWidget->SetReloadProgress(0.f);
		}
		bIsReloading = false;
		GetWorldTimerManager().ClearTimer(ReloadTimerHandle);
	}, ReloadTime, false);
}

void ATank::HandleDestruction()
{
	Super::HandleDestruction();

	bIsAlive = false;
	SetPlayerEnabled(false);
	SetActorHiddenInGame(true);
	SetActorTickEnabled(false);
}

void ATank::SetPlayerEnabled(bool bEnabled)
{
	if (PlayerController)
	{
		if (bEnabled)
		{	
			EnableInput(PlayerController);
		}
		else
		{
			DisableInput(PlayerController);
		}
		PlayerController->SetShowMouseCursor(bEnabled);
	}
}
