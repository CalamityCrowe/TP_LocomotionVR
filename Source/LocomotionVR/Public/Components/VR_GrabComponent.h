// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "VR_GrabComponent.generated.h"


// NEED TO DECLARE A CUSTOM EVENT DISPATCHER FOR THE ON GRABBED AND DROPPED FUNCTIONS
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGrabbedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDroppedDelegate);

UENUM()
enum class EGrabTypes : uint8
{
	None UMETA(DisplayName = "None"),
	Free UMETA(DisplayName = "Free"),
	Snap UMETA(DisplayName = "Snap"),
	Custom UMETA(DisplayName = "Custom")
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class LOCOMOTIONVR_API UVR_GrabComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UVR_GrabComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintPure)
	EControllerHand GetHeldByHand();

	UPROPERTY(BlueprintAssignable)
	FOnGrabbedDelegate OnGrabbed;
	UPROPERTY(BlueprintAssignable)
	FOnDroppedDelegate OnDropped;
protected:
#pragma region Function Decleration

	void SetShouldSimulateDrop();
	void SetPrimativeCompPhysics(bool bisSimulated);

	void AttachParentToController(class UMotionControllerComponent* MotionController);

#pragma endregion

#pragma region Variable Decleration

	bool bisHeld;
	bool bisSimulatedOnDrop;

	FRotator PrimaryGrabRelativeRotation;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EGrabTypes grabType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Haptic Feedback")
	TObjectPtr<class UHapticFeedbackEffect_Base> OnGrabHapticFeedback; // this gets assigned in the engine and helps register that the player has grabbed the object in the world
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Haptic Feedback")
	TObjectPtr<class UHapticFeedbackEffect_Base> OnInteractHapticFeedback; // this gets assigned in the engine and helps register that the player has interacted in the world
	TObjectPtr<UMotionControllerComponent> MotionControllerReference;

#pragma endregion

private:
	FDetachmentTransformRules* DetachmentRules;
public:

#pragma region Function Decleration
	UFUNCTION()
	void HandleOnGrabbed();
	UFUNCTION()
	void HandleOnDropped();

	bool TryGrab(class UMotionControllerComponent* MotionController);
	bool TryRelease();

	void SetGrabType(EGrabTypes newType) { grabType = newType; }
	EGrabTypes GetGrabType() const { return grabType; }

	inline TObjectPtr<UHapticFeedbackEffect_Base> GetGrabHapticEffect() { return OnGrabHapticFeedback; }
	inline void SetGrabHapticEffect(UHapticFeedbackEffect_Base* newHaptic) { OnGrabHapticFeedback = newHaptic; }
	inline TObjectPtr<UHapticFeedbackEffect_Base> GetInteractHapticEffect() { return OnInteractHapticFeedback; }
	inline void SetInteractHapticEffect(UHapticFeedbackEffect_Base* newHaptic) { OnInteractHapticFeedback = newHaptic; }
#pragma endregion
};
