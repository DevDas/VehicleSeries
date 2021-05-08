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
#include "Components/BoxComponent.h"
#include "VehicleSeries/ThirdPerson/TP_ThirdPersonCharacter.h"
#include "Components/CapsuleComponent.h"
#include "VehicleSeries/VehicleLibrary.h"
#include "Blueprint/UserWidget.h"
#include "PhysicalMaterials\PhysicalMaterial.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "FrontWheelBase.h"
#include "RareWheelBase.h"
#include "Kismet/KismetMathLibrary.h"

AVehicleBase::AVehicleBase()
{
	PrimaryActorTick.bCanEverTick = true;

#pragma region Components

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

	EnterCollisionComp = CreateDefaultSubobject<UBoxComponent>(FName("EnterCollisionComp"));
	EnterCollisionComp->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	EnterCollisionComp->SetRelativeLocation(FVector(40.f, 0.f, 100.f));
	EnterCollisionComp->SetBoxExtent(FVector(75.f, 150.f, 50.f));

	EngineSound = CreateDefaultSubobject<UAudioComponent>(FName("EngineSound"));
	EngineSound->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	EngineSound->bAutoActivate = false;

	SkidSFX = CreateDefaultSubobject<UAudioComponent>(FName("SkidSFX"));
	SkidSFX->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	SkidSFX->bAutoActivate = false;

	SitComp = CreateDefaultSubobject<USceneComponent>(FName("SitComp"));
	SitComp->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
#pragma endregion Components

#pragma region VehicleMovement

	Vehicle4W = CastChecked<UWheeledVehicleMovementComponent4W>(GetVehicleMovement());

	if (Vehicle4W)
	{
		Vehicle4W->Mass = 2500.f;

		TSubclassOf<UFrontWheelBase> FrontWheelClass;
		TSubclassOf<URareWheelBase> BackWheelClass;
		ConstructorHelpers::FClassFinder<UFrontWheelBase> FrontWheel(TEXT("/Game/Blueprints/FrontWheel"));
		ConstructorHelpers::FClassFinder<URareWheelBase> BackWheel(TEXT("/Game/Blueprints/RareWheel"));
		if (FrontWheel.Class && BackWheel.Class)
		{
			FrontWheelClass = FrontWheel.Class;
			BackWheelClass = BackWheel.Class;
		}

		FWheelSetup ZERO;
		FWheelSetup ONE;
		FWheelSetup TWO;
		FWheelSetup THREE;
		ZERO.WheelClass = FrontWheelClass;
		ONE.WheelClass = FrontWheelClass;
		TWO.WheelClass = BackWheelClass;
		THREE.WheelClass = BackWheelClass;

		ZERO.BoneName = FName("Wheel_Front_Left");
		ONE.BoneName = FName("Wheel_Front_Right");
		TWO.BoneName = FName("Wheel_Rear_Left");
		THREE.BoneName = FName("Wheel_Rear_Right");

		TWO.bDisableSteering = THREE.bDisableSteering = true;

		Vehicle4W->WheelSetups.Empty();
		Vehicle4W->WheelSetups.Add(ZERO);
		Vehicle4W->WheelSetups.Add(ONE);
		Vehicle4W->WheelSetups.Add(TWO);
		Vehicle4W->WheelSetups.Add(THREE);

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

#pragma endregion VehicleMovement

#pragma region TireFX 

	TE_FR = CreateDefaultSubobject<UParticleSystemComponent>(FName("TE_FR"));
	TE_FR->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("Wheel_Front_Right"));

	TE_FL = CreateDefaultSubobject<UParticleSystemComponent>(FName("TE_FL"));
	TE_FL->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("Wheel_Front_Left"));

	TE_BL = CreateDefaultSubobject<UParticleSystemComponent>(FName("TE_BL"));
	TE_BL->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("Wheel_Rear_Left"));

	TE_BR = CreateDefaultSubobject<UParticleSystemComponent>(FName("TE_BR"));
	TE_BR->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("Wheel_Rear_Right"));


#pragma endregion TireFX

#pragma region DefaultObjectSetup
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

#pragma endregion DefaultObjectSetup

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
		UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("Exit", EKeys::F));
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
	PlayerInputComponent->BindAction("Exit", IE_Pressed, this, &AVehicleBase::ExitVehicle);
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

	EnterCollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AVehicleBase::OnEnterOverlap);
	EnterCollisionComp->OnComponentEndOverlap.AddDynamic(this, &AVehicleBase::OnExitOverlap);

	TireEmitters.Add(TE_FL);
	TireEmitters.Add(TE_FR);
	TireEmitters.Add(TE_BL);
	TireEmitters.Add(TE_BR);
}

void AVehicleBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateAirControl(DeltaTime);
	UpdateWheelEffects();
	SkidDetection();

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
		bool bSomerSet = FVector::DotProduct(GetActorUpVector(), FVector::UpVector) < -0.89f;

		if (bInAir || bNotGrounded)
		{
			UInputComponent* InputComp = FindComponentByClass<UInputComponent>();
			if (InputComp)
			{
				float ForwadInput = InputComp->GetAxisValue("Forward");
				float Steerinput = InputComp->GetAxisValue("Steer");
				
				AirMovementForceRoll = !bInAir && bNotGrounded ? 20.f : 3.f;
				if (bSomerSet) AirMovementForceRoll = 20.f;

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

#pragma region FX

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

void AVehicleBase::ActivateEngineSound()
{ 
	if (EngineSound) EngineSound->Activate(true);
}

#pragma endregion FX

void AVehicleBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (PlayerUIClass)
	{
		PlayerHUD = CreateWidget<UUserWidget>(GetWorld(), PlayerUIClass);
		if (PlayerHUD)
		{
			PlayerHUD->AddToViewport();
		}
	}

	if (EngineStartSound)
	{
		if (!bDebugMute)
		{
			FTimerHandle Handle_EngineSound;
			GetWorld()->GetTimerManager().SetTimer(Handle_EngineSound, this, &AVehicleBase::ActivateEngineSound, 1.5f);
			UGameplayStatics::PlaySound2D(GetWorld(), EngineStartSound);
		}
	}
}

void AVehicleBase::UnPossessed()
{
	Super::UnPossessed();

	if (PlayerHUD)
	{
		PlayerHUD->RemoveFromParent();
	}
	if (Vehicle4W) Vehicle4W->StopMovementImmediately();
	if (EngineSound) EngineSound->Stop();
}

void AVehicleBase::OnEnterOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ATP_ThirdPersonCharacter* Player{ Cast<ATP_ThirdPersonCharacter>(OtherActor) };
	if (Player)
	{
		Player->CarAvailable(true, this);
	}
}

void AVehicleBase::OnExitOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ATP_ThirdPersonCharacter* Player{ Cast<ATP_ThirdPersonCharacter>(OtherActor) };
	if (Player)
	{
		Player->CarAvailable(false, nullptr);
	}
}

void AVehicleBase::ExitVehicle()
{
	if (Driver)
	{
		Driver->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
		Driver->SetActorLocation(UVehicleLibrary::WorldToLocal_Change_LocalToWorld(this, SitComp->GetComponentLocation(), 0.f, -150.f, 100.f));
		Driver->GetCapsuleComponent()->SetCollisionProfileName("Pawn", true);
		Driver->GetMesh()->SetCollisionProfileName("CharacterMesh", true);
		Driver->GetMesh()->SetWorldScale3D(FVector(1.f, 1.f, 1.f));
		Driver->GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
		Driver->bInCar = false;
		UGameplayStatics::GetPlayerController(GetWorld(), 0)->Possess(Driver);
		Driver = nullptr;
	}
}

bool AVehicleBase::IsVehicleInAir()
{
	for (UVehicleWheel* Wheel : Vehicle4W->Wheels)
	{
		if (Wheel)
		{
			if (!Wheel->IsInAir())
			{
				return false;
			}	
		}
	}
	return true;
}

void AVehicleBase::UpdateWheelEffects()
{
	if (IsVehicleInAir())
	{
		bLastTimeTireStatus = true; // Vehicle Was Jumped
	}
	else
	{
		if (bLastTimeTireStatus)
		{
			bLastTimeTireStatus = false;
			if (UVehicleLibrary::GetMaxSuspensionForce(Vehicle4W) > MaxSpringFoce)
			{
				UE_LOG(LogTemp, Warning, TEXT("%f"), UVehicleLibrary::GetMaxSuspensionForce(Vehicle4W))
				//UGameplayStatics::PlaySoundAtLocation()
			}
		}
	}

	for (int32 Index = 0; Index < UVehicleLibrary::GetPhysicalMaterialsUnderTires(Vehicle4W).Num(); ++Index)
	{
		UPhysicalMaterial* ElementAtIndex = UVehicleLibrary::GetPhysicalMaterialsUnderTires(Vehicle4W)[Index];
		if (ElementAtIndex)
		{
			if (Vehicle4W->Wheels[Index]->IsInAir())
			{
				TireEmitters[Index]->Deactivate();	
				UE_LOG(LogTemp, Warning, TEXT("Air"))
			}
			else
			{
				UParticleSystem* ParticleCurrent = TireEmitterMap.FindRef(ElementAtIndex);
				if (ParticleCurrent)
				{
					if (ParticleCurrent == TireEmitters[Index]->Template)
					{
						if (!TireEmitters[Index]->IsActive())
						{
							TireEmitters[Index]->Activate();
						}
					}
					else
					{
						TireEmitters[Index]->SetTemplate(ParticleCurrent);
					}
				}
			}
		}
	}
}

void AVehicleBase::SkidDetection()
{
	if (!SkidSFX) return;

	bool bIsStopped = GetVelocity().SizeSquared2D() < (SlipVelocityThreshold * SlipVelocityThreshold);
	bool bAreTireSkidding = UVehicleLibrary::CheckSlipThreshold(Vehicle4W, 0.8f, 0.8f);
	bool bIsInAir = IsVehicleInAir();
	bool bWantsToSkid = !bIsInAir && !bIsStopped && bAreTireSkidding;

	if (bWantsToSkid && !bIsSkidding)
	{
		bIsSkidding = true;
		SkidSFX->Play();
	}
	else if (bIsSkidding && !bWantsToSkid)
	{
		bIsSkidding = false;
		SkidSFX->FadeOut(1.f, 0.f);
	}
}