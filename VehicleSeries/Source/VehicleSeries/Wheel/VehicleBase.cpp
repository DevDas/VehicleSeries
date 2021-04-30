// ©devdastour All Right Reserved 2019-2021, Current Author = DevDas.


#include "VehicleBase.h"
#include "GameFramework/PlayerInput.h"
#include "Components/InputComponent.h"
#include "WheeledVehicleMovementComponent4W.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"

AVehicleBase::AVehicleBase()
{
	AzuiAzimuthComp = CreateDefaultSubobject<USceneComponent>(FName("AzuiAzimuthComp"));
	AzuiAzimuthComp->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	AzuiAzimuthComp->SetRelativeLocation(FVector(103.408440f, -0.000041f, 95.176765f));

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(FName("SpringArmComp"));
	SpringArmComp->AttachToComponent(AzuiAzimuthComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	SpringArmComp->SetRelativeRotation(FRotator(-14.374288f, 0.000011f, 0.000000f));
	SpringArmComp->TargetArmLength = 622.156860f;
	SpringArmComp->bUsePawnControlRotation = false;
	SpringArmComp->bInheritRoll = false;

	TPPCam = CreateDefaultSubobject<UCameraComponent>(FName("TPPCam"));
	TPPCam->AttachToComponent(SpringArmComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	TPPCam->SetRelativeRotation(FRotator(-8.437481f, 0.000000f, 0.000000f));

	FPPCam = CreateDefaultSubobject<UCameraComponent>(FName("FPPCam"));
	FPPCam->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	FPPCam->bAutoActivate = false;
	FPPCam->SetRelativeLocation(FVector(7.f, -31.f, 120.f));
	FPPCam->SetRelativeRotation(FRotator(-2.812500f, 0.000000f, 0.000000f));
}

#pragma region PlayerInput

static void BindPawnInputs()
{
	static bool bindingsAdded = false;

	if (bindingsAdded == false)
	{
		bindingsAdded = true;

		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("Forward", EKeys::W, 1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("Forward", EKeys::S, -1.f));

		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("Steer", EKeys::A, -1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("Steer", EKeys::D, 1.f));

		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("LookRight", EKeys::MouseX, 1.f));

		UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("GearUp", EKeys::LeftShift));
		UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("GearDown", EKeys::C));
		UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("HandBrake", EKeys::SpaceBar));

		UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("Toggle", EKeys::V));
	}
}

void AVehicleBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent != nullptr);

	Super::SetupPlayerInputComponent(PlayerInputComponent);

	BindPawnInputs();

	PlayerInputComponent->BindAxis("Forward", this, &AVehicleBase::Forward);
	PlayerInputComponent->BindAxis("Steer", this, &AVehicleBase::Steer);

	PlayerInputComponent->BindAxis("LookRight", this, &AVehicleBase::LookRight);

	PlayerInputComponent->BindAction("HandBrake", IE_Pressed, this, &AVehicleBase::HandBrake);
	PlayerInputComponent->BindAction("HandBrake", IE_Released, this, &AVehicleBase::ReleaseHandBrake);

	PlayerInputComponent->BindAction("GearUp", IE_Pressed, this, &AVehicleBase::GearUp);
	PlayerInputComponent->BindAction("GearDown", IE_Pressed, this, &AVehicleBase::GearDown);

	PlayerInputComponent->BindAction("Toggle", IE_Pressed, this, &AVehicleBase::ToggleCamera);
}

void AVehicleBase::Forward(float AxisVal)
{
	GetVehicleMovementComponent()->SetThrottleInput(AxisVal);
}

void AVehicleBase::Steer(float AxisVal)
{
	GetVehicleMovementComponent()->SetSteeringInput(AxisVal);
}

void AVehicleBase::HandBrake()
{
	GetVehicleMovementComponent()->SetHandbrakeInput(true);
}

void AVehicleBase::ReleaseHandBrake()
{
	GetVehicleMovementComponent()->SetHandbrakeInput(false);
}

void AVehicleBase::ToggleCamera()
{
	if (TPPCam && FPPCam)
	{
		TPPCam->ToggleActive();
		FPPCam->ToggleActive();
	}
}

void AVehicleBase::LookRight(float AxisVal)
{
	if (!FPPCam && !TPPCam && !AzuiAzimuthComp) return;

	if (FPPCam->IsActive())
	{
		FPPCam->AddLocalRotation(FRotator(0.f, AxisVal * GetWorld()->DeltaTimeSeconds * 50.f, 0.f));
		FPPCam->SetRelativeRotation(FRotator(0.f, FMath::Clamp(FPPCam->GetRelativeRotation().Yaw, -80.f, 80.f), 0.f));
	}
	else
	{
		AzuiAzimuthComp->AddLocalRotation(FRotator(0.f, AxisVal * GetWorld()->DeltaTimeSeconds * 50.f, 0.f));
	}
}

void AVehicleBase::GearUp()
{

}

void AVehicleBase::GearDown()
{

}

#pragma endregion PlayerInput