// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/VR_GrabComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "MotionControllerComponent.h" // this is used in order to get the motion controllers in the game
#include "Kismet/GameplayStatics.h" // used for getting the players controllers

// Sets default values for this component's properties
UVR_GrabComponent::UVR_GrabComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	DetachmentRules = new FDetachmentTransformRules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, true);
	DetachmentRules->LocationRule = EDetachmentRule::KeepWorld;
	DetachmentRules->RotationRule = EDetachmentRule::KeepWorld;
	DetachmentRules->ScaleRule = EDetachmentRule::KeepWorld;
	DetachmentRules->bCallModify = true;

	bisHeld = false;

	grabType = EGrabTypes::None;

	// ...
}


// Called when the game starts
void UVR_GrabComponent::BeginPlay()
{
	Super::BeginPlay();
	SetShouldSimulateDrop();
	SetPrimativeCompPhysics(true);
	if (UPrimitiveComponent* PrimComponent = Cast<UPrimitiveComponent>(GetAttachParent()))
	{
		PrimComponent->SetCollisionProfileName(FName(TEXT("PhysicsActor")), true); // sets up the collison profiles and tells it to update the overlaps of the object
	}
}


// Called every frame
void UVR_GrabComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


	// ...
}

EControllerHand UVR_GrabComponent::GetHeldByHand()
{
	if (MotionControllerReference->MotionSource == FName(TEXT("Left")))
	{
		return EControllerHand::Left;
	}
	else
	{
		return EControllerHand::Right;
	}
}

void UVR_GrabComponent::SetShouldSimulateDrop()
{
	if (UPrimitiveComponent* PrimComponent = Cast<UPrimitiveComponent>(GetAttachParent()))
	{
		if (PrimComponent->IsAnySimulatingPhysics())
		{
			bisSimulatedOnDrop = true;
		}
	}
}

void UVR_GrabComponent::SetPrimativeCompPhysics(bool bisSimulated)
{
	if (UPrimitiveComponent* PrimComponent = Cast<UPrimitiveComponent>(GetAttachParent())) 
	{
		PrimComponent->SetSimulatePhysics(bisSimulated);
	}
}

void UVR_GrabComponent::AttachParentToController(UMotionControllerComponent* MotionController)
{
	FAttachmentTransformRules AttachmentRules = FAttachmentTransformRules::KeepWorldTransform;
	AttachmentRules.bWeldSimulatedBodies = true;
	if (GetAttachParent()->AttachToComponent(MotionController, AttachmentRules, FName(TEXT("None")))) 
	{
		
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to attach to controller"));
	}
}

void UVR_GrabComponent::HandleOnGrabbed()
{
	if (OnGrabbed.IsBound())
	{
		OnGrabbed.Broadcast();
	}
}

void UVR_GrabComponent::HandleOnDropped()
{
	if (OnDropped.IsBound())
	{
		OnDropped.Broadcast();
	}
}

bool UVR_GrabComponent::TryGrab(UMotionControllerComponent* MotionController)
{
	switch (grabType)
	{
	case EGrabTypes::None:
		break;
	case EGrabTypes::Free:
	{
		SetPrimativeCompPhysics(false);
		AttachParentToController(MotionController);
		bisHeld = true;
		break;
	}
	case EGrabTypes::Snap:
	{
		SetPrimativeCompPhysics(false);
		AttachParentToController(MotionController);
		bisHeld = true;

		const FRotator InvertedRotation = UKismetMathLibrary::NegateRotator(GetRelativeRotation());
		GetAttachParent()->SetRelativeRotation(InvertedRotation);


		const FVector ParentLocation = GetAttachParent()->GetComponentLocation();

		FVector newLocation = (GetComponentLocation() - ParentLocation) * -1;

		newLocation = MotionController->GetComponentLocation() + newLocation;

		GetAttachParent()->SetWorldLocation(newLocation);

		break;
	}
	case EGrabTypes::Custom:
		bisHeld = true;
		break;
	}

	if (bisHeld == false)
	{
		return false;
	}

	MotionControllerReference = MotionController;

	HandleOnGrabbed();

	if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		PC->PlayHapticEffect(GetGrabHapticEffect(), GetHeldByHand(), 1, false);
	}
	return true;
}

bool UVR_GrabComponent::TryRelease()
{
	switch (grabType)
	{
	case EGrabTypes::None:
		break;
	case EGrabTypes::Free:
	{
		if (bisSimulatedOnDrop)
		{
			SetPrimativeCompPhysics(true);
		}
		GetAttachParent()->DetachFromComponent(*DetachmentRules);
		bisHeld = false;
		break;
	}
	case EGrabTypes::Snap:
	{
		if (bisSimulatedOnDrop)
		{
			SetPrimativeCompPhysics(true);
		}
		GetAttachParent()->DetachFromComponent(*DetachmentRules);
		bisHeld = false;
		break;
	}
	default:
		break;
	}
	if (bisHeld == true)
	{
		return false;
	}
	else
	{
		HandleOnDropped();
		bisHeld = false;
		return true;
	}
}
