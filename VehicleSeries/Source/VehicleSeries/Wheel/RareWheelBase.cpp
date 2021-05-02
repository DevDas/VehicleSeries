// ©devdastour All Right Reserved 2019-2021, Current Author = DevDas.


#include "RareWheelBase.h"
#include "UObject/ConstructorHelpers.h"
#include "TireConfig.h"

URareWheelBase::URareWheelBase()
{
	ConstructorHelpers::FObjectFinder<UTireConfig> RareTireConfig(TEXT("/Game/Blueprints/RareWheels_Data"));
	if (RareTireConfig.Object)
	{
		TireConfig = RareTireConfig.Object;
	}
	SteerAngle = 0.f;
	ShapeRadius = 31.f;
	ShapeWidth = 30.f;
	LatStiffValue = 10.f;
}