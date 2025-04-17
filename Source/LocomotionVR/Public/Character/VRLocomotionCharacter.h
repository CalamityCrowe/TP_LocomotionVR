// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "VRLocomotionCharacter.generated.h"

class UCameraComponent;
class USceneComponent;
class UMannequin_hands;
class UMotionControllerComponent;
class UVR_GrabComponent;
class UInputMappingContext;
class UInputAction; 


USTRUCT(BlueprintType)
struct FMappingContext
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input Mapping")
	TObjectPtr<UInputMappingContext> Context;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input Mapping", meta = (UIMin = "0", ClampMin = "0"))
	int32 Priority;
};
UCLASS()
class LOCOMOTIONVR_API AVRLocomotionCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AVRLocomotionCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	void AllignColliderToHMD();
	UFUNCTION()
	virtual void MoveForward(const FInputActionValue& Value);
	UFUNCTION()
	virtual void MoveRight(const FInputActionValue& Value);
	UFUNCTION()
	virtual void Turn(const FInputActionValue& Value);
	UFUNCTION()
	virtual void SnapTurn(const FInputActionValue& Value);
	UFUNCTION()
	virtual void ToggleSnapTurn(const FInputActionValue& Value)
	{
		bSnapTurning = !bSnapTurning;
	}
	UFUNCTION()
	void GrabObjectLeft(const FInputActionValue& Value);
	void GrabObjectRight(const FInputActionValue& Value);
	void ReleaseObjectLeft(const FInputActionValue& Value);
	void ReleaseObjectRight(const FInputActionValue& Value);


	UMannequin_hands* GetLeftHand() const { return LeftHand;  };
	UMannequin_hands* GetRightHand() const { return RightHand; };

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UVR_GrabComponent* GetGrabComponentNearController(UMotionControllerComponent* Controller);

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Camera")
	TObjectPtr<UCameraComponent> Camera;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "VR Origin")
	TObjectPtr<USceneComponent> VROrigin;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Motion Controller")
	TObjectPtr<UMotionControllerComponent> RightController;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Motion Controller")
	TObjectPtr<UMotionControllerComponent> LeftController;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Motion Controller")
	TObjectPtr<UMannequin_hands> RightHand;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Motion Controller")
	TObjectPtr<UMannequin_hands> LeftHand;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Motion Controller")
	TObjectPtr<UVR_GrabComponent> RightGrab;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Motion Controller")
	TObjectPtr<UVR_GrabComponent> LeftGrab;

	UPROPERTY(EditDefaultsOnly, BluePrintReadWrite, Category = "Motion Controllers", meta = (AllowPrivateAccess = true))
	float GrabRadius;
	UPROPERTY(EditDefaultsOnly, BluePrintReadWrite, Category = "Motion Controllers", meta = (AllowPrivateAccess = true))
	float LocalNearestDistance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Movement")
	bool bSnapTurning;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true, ClampMin = "10", ClampMax = "90.0", UIMin = "10.0", UIMax = "90.0"), Category = "Movement")
	float SnapTurnScale;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true, ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"), Category = "Movement")
	float TurnScale;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true, ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"), Category = "Movement")
	float WalkScale;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	FVector EndPoint; 

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inputs", meta = (AllowPrivateAccess = true))
	TArray<FMappingContext> MappingContexts;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inputs", meta = (AllowPrivateAccess = true))
	TArray<TObjectPtr<UInputAction>>LeftControllerActions; 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inputs", meta = (AllowPrivateAccess = true))
	TArray<TObjectPtr<UInputAction>>RightControllerActions;
};
