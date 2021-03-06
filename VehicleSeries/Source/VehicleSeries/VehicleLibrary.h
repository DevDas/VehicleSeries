// ©devdastour All Right Reserved 2019-2021, Current Author = DevDas.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "VehicleLibrary.generated.h"

class UWheeledVehicleMovementComponent;
class UPhysicalMaterial;

/**
 * 
 */
UCLASS()
class VEHICLESERIES_API UVehicleLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, Category = "Drive")
		static TArray<UPhysicalMaterial*> GetPhysicalMaterialsUnderTires(UWheeledVehicleMovementComponent* VehMovement);

	UFUNCTION(BlueprintCallable, Category = "Drive")
		static float GetMaxSuspensionForce(UWheeledVehicleMovementComponent* VehMovement);

	UFUNCTION(BlueprintCallable, Category = "Drive")
		static bool CheckSlipThreshold(UWheeledVehicleMovementComponent* VehMovement, float LongSlipThresh, float LatSlipThresh);

	UFUNCTION(BlueprintCallable, Category = "Co-Ordinates")
	static FVector WorldToLocal_Change_LocalToWorld(AActor* Actor, FVector Location, float XChange, float YChange, float ZChange);
};
