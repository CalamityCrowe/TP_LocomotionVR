// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "Mannequin_hands.generated.h"

/**
 * 
 */
UCLASS()
class LOCOMOTIONVR_API UMannequin_hands : public USkeletalMeshComponent
{
	GENERATED_BODY()

public: 
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	bool bMirror; 
};
