// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasePawn.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "EnhancedInputComponent.h"
#include "Tank.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;


/**
 * 
 */
UCLASS()
class BATTLEBLASTER_API ATank : public ABasePawn
{
	GENERATED_BODY()
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Sets default values for this pawn's properties
	ATank();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* TurnAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* FireAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float Speed = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float TurnRate = 45.f;

	/** Pointer to the player controller assigned to this character */
	TObjectPtr<APlayerController> PlayerController;

	bool bIsAlive = true;

	void MoveInput(const FInputActionValue& Value);
	void TurnInput(const FInputActionValue& Value);

	/** Handles joypad aim */
	void StickAim(const FInputActionValue& Value);

	/** Handles mouse aim */
	void MouseAim(const FInputActionValue& Value);

	/** Handles aim inputs from both input actions and touch interface */
	UFUNCTION(BlueprintCallable, Category = "Input")
	void DoAim(float AxisX, float AxisY);

	void HandleDestruction();
	void SetPlayerEnabled(bool bEnabled);

protected:
	/** Aim Yaw Angle in degrees */
	float AimAngle = 0.0f;

	/** If true, the player is using mouse aim */
	bool bUsingMouse = false;

	/** Gamepad aim input action */
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* StickAimAction;

	/** Mouse aim input action */
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* MouseAimAction;
};
