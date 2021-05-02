// ©devdastour All Right Reserved 2019-2021, Current Author = DevDas.


#include "FrontWheelBase.h"
#include "UObject/ConstructorHelpers.h"
#include "TireConfig.h"

UFrontWheelBase::UFrontWheelBase()
{
	ConstructorHelpers::FObjectFinder<UTireConfig> FrontTireConfig(TEXT("/Game/Blueprints/FrontWheels_Data"));
	if (FrontTireConfig.Object)
	{
		TireConfig = FrontTireConfig.Object;
	}
	SteerAngle = 45.f;
	ShapeRadius = 31.f;
	ShapeWidth = 30.f;
	bAffectedByHandbrake = true;
	LatStiffValue = 10.f;
	LongStiffValue = 1600.f;
}
