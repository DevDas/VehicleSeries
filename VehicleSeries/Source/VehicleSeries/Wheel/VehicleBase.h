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

	void Forward(float AxisVal);
	void Steer(float AxisVal);

	void HandBrake();
	void ReleaseHandBrake();

	void GearUp();
	void GearDown();
	void ToggleCamera();
	void LookRight(float AxisVal);

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		class USceneComponent* AzuiAzimuthComp = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USpringArmComponent* SpringArmComp = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UCameraComponent* TPPCam = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UCameraComponent* FPPCam = nullptr;
};
