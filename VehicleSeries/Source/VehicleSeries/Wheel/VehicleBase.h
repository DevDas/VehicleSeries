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
	virtual void UnPossessed();

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

	void UpdateWheelEffects();
	void SkidDetection();

	class ATP_ThirdPersonCharacter* Driver = nullptr;
	class USceneComponent* GetSitPoint() { return SitComp; }

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		class UAudioComponent* SkidSFX = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		class UBoxComponent* EnterCollisionComp = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		class USceneComponent* SitComp = nullptr;

#pragma region TireFX 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		class UParticleSystemComponent* TE_FR;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		class UParticleSystemComponent* TE_FL;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		class UParticleSystemComponent* TE_BL;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		class UParticleSystemComponent* TE_BR;

#pragma endregion TireFX

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AirMovement")
		float AirMovementEffectiveDistance = 200.f;

	UPROPERTY(EditAnywhere)
		bool bDebugMute = true;

	UPROPERTY(EditAnywhere, Category = "Drift")
		float MaxSpringFoce = 500000.f;

	UFUNCTION()
		void OnEnterOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
			UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnExitOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void ExitVehicle();

	bool IsVehicleInAir();

	bool bLastTimeTireStatus = false;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> PlayerUIClass;

	class UUserWidget* PlayerHUD;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drift")
		TMap<class UPhysicalMaterial*, class UParticleSystem*> TireEmitterMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drift")
	TArray<class UParticleSystemComponent*> TireEmitters;

	UPROPERTY(EditAnywhere, Category = "Drift")
		float SlipVelocityThreshold = 600.f;

	bool bIsSkidding = false;
};
