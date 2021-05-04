// ©devdastour All Right Reserved 2019-2021, Current Author = DevDas.


#include "VehicleBase.h"
#include "GameFramework/PlayerInput.h"
#include "Components/InputComponent.h"
#include "WheeledVehicleMovementComponent4W.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"
#include "Components/AudioComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInterface.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"

AVehicleBase::AVehicleBase()
{
	PrimaryActorTick.bCanEverTick = true;

	AzimuthComp = CreateDefaultSubobject<USceneComponent>(FName("AzimuthComp"));
	AzimuthComp->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	AzimuthComp->SetRelativeLocation(FVector(0.f, 0.f, 75.f));

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(FName("SpringArmComp"));
	SpringArmComp->AttachToComponent(AzimuthComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	SpringArmComp->TargetArmLength = 400.f;
	SpringArmComp->bUsePawnControlRotation = false;
	SpringArmComp->bInheritRoll = false;
	SpringArmComp->bEnableCameraLag = true;
	SpringArmComp->bEnableCameraRotationLag = true;
	SpringArmComp->CameraLagSpeed = 10.f;
	SpringArmComp->CameraRotationLagSpeed = 7.f;
	SpringArmComp->CameraLagMaxDistance = 50.f;

	TPPCam = CreateDefaultSubobject<UCameraComponent>(FName("TPPCam"));
	TPPCam->AttachToComponent(SpringArmComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

	FPPCam = CreateDefaultSubobject<UCameraComponent>(FName("FPPCam"));
	FPPCam->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	FPPCam->bAutoActivate = false;
	FPPCam->SetRelativeLocation(FVector(7.f, -31.f, 120.f));
	FPPCam->SetRelativeRotation(FRotator(-2.812500f, 0.000000f, 0.000000f));

	EngineSound = CreateDefaultSubobject<UAudioComponent>(FName("EngineSound"));
	EngineSound->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	EngineSound->bAutoActivate = false;

	Vehicle4W = CastChecked<UWheeledVehicleMovementComponent4W>(GetVehicleMovement());

	if (Vehicle4W)
	{
		Vehicle4W->EngineSetup.TorqueCurve.GetRichCurve()->Reset();
		Vehicle4W->EngineSetup.TorqueCurve.GetRichCurve()->AddKey(0.0f, 400.f);
		Vehicle4W->EngineSetup.TorqueCurve.GetRichCurve()->AddKey(1890.0f, 700.f);
		Vehicle4W->EngineSetup.TorqueCurve.GetRichCurve()->AddKey(9000.0f, 400.f);

		Vehicle4W->SteeringCurve.GetRichCurve()->Reset();
		Vehicle4W->SteeringCurve.GetRichCurve()->AddKey(0.0f, 1.f);
		Vehicle4W->SteeringCurve.GetRichCurve()->AddKey(20.f, 0.95f);
		Vehicle4W->SteeringCurve.GetRichCurve()->AddKey(60.f, 0.9f);
		Vehicle4W->SteeringCurve.GetRichCurve()->AddKey(120.f, 0.7f);

		Vehicle4W->DifferentialSetup.DifferentialType = EVehicleDifferential4W::LimitedSlip_FrontDrive; // Default Is LimitedSliped_4W
	}

	ConstructorHelpers::FObjectFinder<UMaterialInterface> BreakMaterial(TEXT("/Game/Vehicle/M_BreakLight_Inst"));
	if (BreakMaterial.Object)
	{
		BreakLightMaterial = BreakMaterial.Object;
	}

	ConstructorHelpers::FObjectFinder<USoundBase> EngineBeginSound(TEXT("/Game/Sound/waves/sportscar/start_standard"));
	if (EngineBeginSound.Object)
	{
		EngineStartSound = EngineBeginSound.Object;
	}
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
	if (AxisVal < 0.f || bIsHandbreakPressed)
	{
		HandleBreakLights(1.f);
	}
	else
	{
		HandleBreakLights(0.f);
	}
}

void AVehicleBase::Steer(float AxisVal)
{
	GetVehicleMovementComponent()->SetSteeringInput(AxisVal);
}

void AVehicleBase::HandBrake()
{
	GetVehicleMovementComponent()->SetHandbrakeInput(true);
	bIsHandbreakPressed = true;
}

void AVehicleBase::ReleaseHandBrake()
{
	GetVehicleMovementComponent()->SetHandbrakeInput(false);
	bIsHandbreakPressed = false;
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
	if (!FPPCam && !TPPCam && !AzimuthComp) return;

	if (FPPCam->IsActive())
	{
		FPPCam->AddLocalRotation(FRotator(0.f, AxisVal * GetWorld()->DeltaTimeSeconds * 50.f, 0.f));
		FPPCam->SetRelativeRotation(FRotator(0.f, FMath::Clamp(FPPCam->GetRelativeRotation().Yaw, -80.f, 80.f), 0.f));
	}
	else
	{
		AzimuthComp->AddLocalRotation(FRotator(0.f, AxisVal * GetWorld()->DeltaTimeSeconds * 50.f, 0.f));
	}
}

void AVehicleBase::GearUp()
{

}

void AVehicleBase::GearDown()
{

}

#pragma endregion PlayerInput

void AVehicleBase::BeginPlay()
{
	Super::BeginPlay();

	CreateDynamicMaterialBreak();
}

void AVehicleBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateAirControl(DeltaTime);

	if (EngineSound)
	{
		EngineSound->SetFloatParameter(FName("RPM"), GetVehicleMovementComponent()->GetEngineRotationSpeed());
	}
}

void AVehicleBase::UpdateAirControl(float DeltaTime)
{
	if (Vehicle4W)
	{
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);

		FVector TraceStart{ GetActorLocation() + FVector(0.f, 0.f, 50.f) };
		FVector TraceEnd{ GetActorLocation() - FVector(0.f, 0.f, AirMovementEffectiveDistance) };

		FHitResult Hit;
		bool bInAir = !GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, QueryParams);
		bool bNotGrounded = FVector::DotProduct(GetActorUpVector(), FVector::UpVector) < FlippedThreshold;
		bool bFliped = FVector::DotProduct(GetActorUpVector(), FVector::UpVector) < -0.89f;

		if (bInAir || bNotGrounded)
		{
			UInputComponent* InputComp = FindComponentByClass<UInputComponent>();
			if (InputComp)
			{
				float ForwadInput = InputComp->GetAxisValue("Forward");
				float Steerinput = InputComp->GetAxisValue("Steer");
				
				AirMovementForceRoll = !bInAir && bNotGrounded ? 20.f : 3.f;
				if (bFliped) AirMovementForceRoll = 20.f;

				if (UPrimitiveComponent* VehicleMesh = Vehicle4W->UpdatedPrimitive)
				{
					FVector MovementVector = FVector(Steerinput * -AirMovementForceRoll, ForwadInput * AirMovementForcePitch, 0.f) * DeltaTime * RotationStrength;
					FVector NewAngularMovement = GetActorRotation().RotateVector(MovementVector);
					VehicleMesh->SetPhysicsAngularVelocity(NewAngularMovement, true);
				}
			}
		}
	}
}

void AVehicleBase::CreateDynamicMaterialBreak()
{
	TArray<UActorComponent*> BreakLights = GetComponentsByTag(UStaticMeshComponent::StaticClass(), FName("BreakLight"));
	for (auto Component : BreakLights)
	{
		UStaticMeshComponent* Light{ Cast<UStaticMeshComponent>(Component) };
		if (Light)
		{
			UMaterialInstanceDynamic* BreakMaterialInst = Light->CreateDynamicMaterialInstance(0, BreakLightMaterial);
			if (BreakMaterialInst)
			{
				BreakMaterialsInst.Add(BreakMaterialInst);
			}		
		}
	}
}

void AVehicleBase::HandleBreakLights(float Value)
{
	for (UMaterialInstanceDynamic* MatInst : BreakMaterialsInst)
	{
		if (MatInst)
		{
			MatInst->SetScalarParameterValue(FName("Light"), Value);
		}
	}
}

void AVehicleBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (EngineStartSound)
	{
		if (!bDebugMute)
		{
			FTimerHandle Handle_EngineSound;
			GetWorld()->GetTimerManager().SetTimer(Handle_EngineSound, this, &AVehicleBase::ActivateEngineSound, 1.5f);
			UGameplayStatics::PlaySound2D(GetWorld(), EngineStartSound);
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Possessed"))
}

void AVehicleBase::ActivateEngineSound()
{ 
	if (EngineSound) EngineSound->Activate(true);
}