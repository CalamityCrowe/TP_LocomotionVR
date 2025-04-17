// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GrabbableGun.generated.h"

class UVR_GrabComponent;
class USkeletalMeshComponent;
class AVR_Projectiles;
class UInputAction;
class UHapticFeedbackEffect_Curve;

UCLASS()
class LOCOMOTIONVR_API AGrabbableGun : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGrabbableGun();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;


private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = true))
	TObjectPtr<USkeletalMeshComponent> Mesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Scene Components", meta = (AllowPrivateAccess = true))
	TObjectPtr<USceneComponent> MuzzlePoint;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Scene Components", meta = (AllowPrivateAccess = true))
	TObjectPtr<UVR_GrabComponent> GripPoint;
	UPROPERTY(EditDefaultsOnly, Category = "Input", meta = (AllowPrivateAccess = true))
	TObjectPtr<UInputAction> RightAction;
	UPROPERTY(EditDefaultsOnly, Category = "Input", meta = (AllowPrivateAccess = true))
	TObjectPtr<UInputAction> LeftAction;
	UPROPERTY(EditDefaultsOnly, Category = "Sound", meta = (AllowPrivateAccess = true))
	TObjectPtr<USoundBase> FireSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Haptics", meta = (AllowPrivateAccess = true))
	TObjectPtr<UHapticFeedbackEffect_Curve> HapticEffect;



	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectiles", meta = (AllowPrivateAccess = true))
	bool bUseProjectiles = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectiles", meta = (AllowPrivateAccess = true, EditCondition = "bUseProjectiles"))
	TSubclassOf<AVR_Projectiles> ProjectileRef;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectiles", meta = (AllowPrivateAccess = true, EditCondition = "!bUseProjectiles"))
	float TraceDistance = 1000.0f;
	UPROPERTY(BlueprintReadOnly, Category = "Projectiles", meta = (AllowPrivateAccess = true))
	FVector EndPoint;

	UFUNCTION(BlueprintCallable)
	void OnFire();
};
