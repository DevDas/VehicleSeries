// ©devdastour All Right Reserved 2019-2021, Current Author = DevDas.

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehicle.h"
#include "VehicleBase.generated.h"

/**
 * 
 */
UCLASS()
class VEHICLESERIES_API AVehicleBase : public AWheeledVehicle
{
	GENERATED_BODY()

public:

	AVehicleBase();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;

	virtual void PossessedBy(AController* NewController);

	void Forward(float AxisVal);
	void Steer(float AxisVal);

	void HandBrake();
	void ReleaseHandBrake();

	void GearUp();
	void GearDown();
	void ToggleCamera();
	void LookRight(float AxisVal);

	void UpdateAirControl(float DeltaTime);

	void CreateDynamicMaterialBreak();
	void HandleBreakLights(float Value);
	void ActivateEngineSound();

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USceneComponent* AzimuthComp = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USpringArmComponent* SpringArmComp = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UCameraComponent* TPPCam = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UCameraComponent* FPPCam = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		class UAudioComponent* EngineSound = nullptr;

	class UMaterialInterface* BreakLightMaterial;
	TArray<class UMaterialInstanceDynamic*> BreakMaterialsInst;

	class USoundBase* EngineStartSound = nullptr;

	bool bIsHandbreakPressed = false;

	class UWheeledVehicleMovementComponent4W* Vehicle4W = nullptr;

	UPROPERTY(EditAnywhere, Category = "AirMovement")
	float AirMovementForcePitch = 1.8f;

	float AirMovementForceRoll;

	UPROPERTY(EditAnywhere, Category = "AirMovement")
		float RotationStrength = 100.f;

	UPROPERTY(EditAnywhere, Category = "AirMovement", meta = (ClampMin = "0.1", ClampMax = "0.5"))
		float FlippedThreshold = 0.2f;

	UPROPERTY(EditAnywhere, Category = "AirMovement")
		float AirMovementEffectiveDistance = 200.f;

	UPROPERTY(EditAnywhere)
		bool bDebugMute = true;
};
