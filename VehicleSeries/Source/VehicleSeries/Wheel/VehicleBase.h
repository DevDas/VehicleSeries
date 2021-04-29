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

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void Forward(float AxisVal);
	void Steer(float AxisVal);

	void HandBrake();
	void ReleaseHandBrake();

	void GearUp();
	void GearDown();
};
