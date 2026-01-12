// Fill out your copyright notice in the Description page of Project Settings.


#include "Tank.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "InputMappingContext.h"

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

	APlayerController* PlayerController = Cast<APlayerController>(Controller);
	if (PlayerController)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

// Called every frame
void ATank::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	APlayerController* PlayerController = Cast<APlayerController>(Controller);
	if (PlayerController)
	{
		FHitResult HitResult;
		PlayerController->GetHitResultUnderCursor(ECC_Visibility, false, HitResult);
		FVector HitLocation = HitResult.ImpactPoint;
		RotateTurret(HitLocation);
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
		/*EnhancedInputComponent->BindAction(RotateAction, ETriggerEvent::Triggered, this, &ATank::RotateInput);*/
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &ATank::Fire);
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

//void ATank::RotateInput(const FInputActionValue& Value)
//{
//	FVector2D LookAxisValue = Value.Get<FVector2D>();
//	UE_LOG(LogTemp, Warning, TEXT("Rotate Input: X=%f, Y=%f"), LookAxisValue.X, LookAxisValue.Y);
//
//	FVector LookAtTarget = GetActorLocation() + FVector(LookAxisValue.X, LookAxisValue.Y, 0.f) * 1000.f;
//	RotateTurret(LookAtTarget);
//}
