// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/VRLocomotionCharacter.h"

#include "MotionControllerComponent.h" // this is used in order to get the motion controllers in the game
#include "Camera/CameraComponent.h" // gets the camera for using in the game
#include "GameFramework/CharacterMovementComponent.h" // gets the functionallity of the movement component
#include "Components/CapsuleComponent.h"
#include "Components/Mannequin_hands.h"
#include "Components/VR_GrabComponent.h"

#include <Kismet/KismetMathLibrary.h>
#include <Kismet/KismetSystemLibrary.h>
#include <HeadMountedDisplayFunctionLibrary.h>

#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"

#include "UserSettings/EnhancedInputUserSettings.h"

// Sets default values
AVRLocomotionCharacter::AVRLocomotionCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	VROrigin = CreateDefaultSubobject<USceneComponent>(TEXT("VROrigin"));
	VROrigin->SetupAttachment(GetCapsuleComponent());

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(VROrigin);

	LeftController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("Left Motion Controller"));
	RightController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("Right Motion Controller"));

	LeftController->SetupAttachment(VROrigin);
	LeftController->MotionSource = FName(TEXT("Left"));
	RightController->SetupAttachment(VROrigin);
	RightController->MotionSource = FName(TEXT("Right"));

	LeftHand = CreateOptionalDefaultSubobject<UMannequin_hands>(TEXT("Left Hand"));
	LeftHand->SetupAttachment(LeftController);
	LeftHand->SetRelativeLocation(FVector(-2.98126, -3.5, 4.561753));
	LeftHand->SetRelativeRotation(FRotator(-25, -179.999908, 89.99998));
	LeftHand->bMirror = true;

	RightHand = CreateOptionalDefaultSubobject<UMannequin_hands>(TEXT("Right Hand"));
	RightHand->SetupAttachment(RightController);
	RightHand->SetRelativeLocation(FVector(-2.98126, 3.5, 4.561753));
	RightHand->SetRelativeRotation(FRotator(-25, 0, 89.999999));
	RightHand->bMirror = false;
}

// Called when the game starts or when spawned
void AVRLocomotionCharacter::BeginPlay()
{
	Super::BeginPlay();

	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition(0, EOrientPositionSelector::Orientation); // resets the orientation and position of the player

}
// Called every frame
void AVRLocomotionCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	AllignColliderToHMD();
}

// Called to bind functionality to input
void AVRLocomotionCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (MappingContexts.Num() < 1 || RightControllerActions.Num() < 1 || LeftControllerActions.Num() < 1)
	{
#if UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("Mapping Contexts or Controller Actions are not set up correctly"));
#endif
		return;
	}
	if (APlayerController* PC = Cast<APlayerController>(GetController())) 
	{
		if (ULocalPlayer* LP = PC->GetLocalPlayer()) 
		{
			if (UEnhancedInputLocalPlayerSubsystem* InputSystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>()) 
			{
				InputSystem->ClearAllMappings();
				for (FMappingContext map : MappingContexts) 
				{
					InputSystem->AddMappingContext(map.Context, map.Priority);
				}
			}
		}
	}
	if(UEnhancedInputComponent* PEI = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		
		// left controller
		PEI->BindAction(LeftControllerActions[0], ETriggerEvent::Triggered, this, &AVRLocomotionCharacter::MoveForward);
		PEI->BindAction(LeftControllerActions[1], ETriggerEvent::Triggered, this, &AVRLocomotionCharacter::MoveRight);
		PEI->BindAction(LeftControllerActions[2], ETriggerEvent::Started, this, &AVRLocomotionCharacter::GrabObjectLeft);
		PEI->BindAction(LeftControllerActions[2], ETriggerEvent::Completed, this, &AVRLocomotionCharacter::ReleaseObjectLeft);
		// right controller
		PEI->BindAction(RightControllerActions[0], ETriggerEvent::Triggered, this, &AVRLocomotionCharacter::Turn);
		PEI->BindAction(RightControllerActions[0], ETriggerEvent::Started, this, &AVRLocomotionCharacter::SnapTurn);
		PEI->BindAction(RightControllerActions[1], ETriggerEvent::Started, this, &AVRLocomotionCharacter::ToggleSnapTurn);
		PEI->BindAction(RightControllerActions[2], ETriggerEvent::Started, this, &AVRLocomotionCharacter::GrabObjectRight);
		PEI->BindAction(RightControllerActions[2], ETriggerEvent::Completed, this, &AVRLocomotionCharacter::ReleaseObjectRight);
		PEI->BindAction(RightControllerActions[3], ETriggerEvent::Started, this, &ACharacter::Jump);
	}


}

UVR_GrabComponent* AVRLocomotionCharacter::GetGrabComponentNearController(UMotionControllerComponent* MotionRef)
{

	LocalNearestDistance = 1000000.0f; // reset the distance to a high value
	UVR_GrabComponent* LocalGrabComp = nullptr;

	FHitResult hitResult;

	FVector LocalGripPos = MotionRef->GetComponentLocation();


	TArray<TEnumAsByte<EObjectTypeQuery>> traceObjects;
	traceObjects.Add(UEngineTypes::ConvertToObjectType(ECC_PhysicsBody));
	const TArray<AActor*> ignoreActors{ this };

	bool bHasHit = UKismetSystemLibrary::SphereTraceSingleForObjects(GetWorld(), LocalGripPos, LocalGripPos, GrabRadius, traceObjects, false, ignoreActors, EDrawDebugTrace::Persistent, hitResult, true);
	if (bHasHit)
	{
		TArray<UVR_GrabComponent*> GrabPoints;
		hitResult.GetActor()->GetComponents(GrabPoints);
		if (GrabPoints.Num() > 0)
		{
			for (int i = 0; i < GrabPoints.Num(); ++i)
			{
				FVector comLoc = GrabPoints[i]->GetComponentLocation();
				comLoc -= LocalGripPos;
				float sqLength = comLoc.SquaredLength();
				if (sqLength <= LocalNearestDistance)
				{
					LocalNearestDistance = sqLength;
					LocalGrabComp = GrabPoints[i];
				}
			}
		}
	}

	return LocalGrabComp;
}

void AVRLocomotionCharacter::AllignColliderToHMD()
{
	FVector CamLoc = Camera->GetComponentLocation();
	FVector ActLoc = GetActorLocation();
	if (FVector::DistSquared(CamLoc,ActLoc) > 1000)
	{
		FVector newOffset = CamLoc - ActLoc;
		newOffset.Z = 0;
		GetCapsuleComponent()->AddWorldOffset(newOffset);

		VROrigin->AddWorldOffset(-newOffset);
	}
}

void AVRLocomotionCharacter::MoveForward(const FInputActionValue& Value)
{
	float MovementValue = Value.Get<float>();
	AddMovementInput(Camera->GetForwardVector(), MovementValue * WalkScale);
}

void AVRLocomotionCharacter::MoveRight(const FInputActionValue& Value)
{
	float MovementValue = Value.Get<float>();
	AddMovementInput(Camera->GetRightVector(), MovementValue * WalkScale);
}

void AVRLocomotionCharacter::Turn(const FInputActionValue& Value)
{
	if (bSnapTurning == false)
	{
		float TurnValue = Value.Get<float>();
		AddControllerYawInput(TurnValue);
	}

}

void AVRLocomotionCharacter::SnapTurn(const FInputActionValue& Value)
{
	if (bSnapTurning == true)
	{
		float TurnValue = Value.Get<float>();
		if (TurnValue > 0)
		{
			AddControllerYawInput(SnapTurnScale);
		}
		else
		{
			AddControllerYawInput(-SnapTurnScale);
		}
	}
}

void AVRLocomotionCharacter::GrabObjectLeft(const FInputActionValue& Value)
{
	if (UVR_GrabComponent* nearestComp = GetGrabComponentNearController(LeftController))
	{
		if (nearestComp->TryGrab(LeftController))
		{
			LeftGrab = nearestComp;
			if (LeftGrab == RightGrab) 
			{
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Dropped Right"));
				RightGrab = nullptr;
				RightHand->SetVisibility(true);
			}
			LeftHand->SetVisibility(false);
		}
	}
}

void AVRLocomotionCharacter::GrabObjectRight(const FInputActionValue& Value)
{
	if (UVR_GrabComponent* nearestComp = GetGrabComponentNearController(RightController))
	{
		if (nearestComp->TryGrab(RightController))
		{
			RightGrab = nearestComp;
			if (RightGrab == LeftGrab) 
			{
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Dropped Left"));
				LeftGrab = nullptr;
				LeftHand->SetVisibility(true);
			}
			RightHand->SetVisibility(false);
		}
	}
}

void AVRLocomotionCharacter::ReleaseObjectLeft(const FInputActionValue& Value)
{
	if (LeftGrab != nullptr)
	{
		if (LeftGrab->TryRelease())
		{
			LeftGrab = nullptr;
			LeftHand->SetVisibility(true);
		}
	}
}

void AVRLocomotionCharacter::ReleaseObjectRight(const FInputActionValue& Value)
{
	if (RightGrab != nullptr)
	{
		if (RightGrab->TryRelease())
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Dropped Right"));
			RightGrab = nullptr;
			RightHand->SetVisibility(true);
		}
	}
}

