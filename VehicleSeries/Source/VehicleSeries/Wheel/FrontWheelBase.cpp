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
	SteerAngle = 55.f;
	ShapeWidth = 18.f;
	bAffectedByHandbrake = false;
	LatStiffValue = 20.f;
}
