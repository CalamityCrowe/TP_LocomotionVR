// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/GrabbableGun.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/VR_GrabComponent.h"
#include "Actors/VR_Projectiles.h"
#include "Haptics/HapticFeedbackEffect_Curve.h"
#include <Kismet/GameplayStatics.h>


// Sets default values
AGrabbableGun::AGrabbableGun()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	MuzzlePoint = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzlePoint"));
	MuzzlePoint->SetupAttachment(Mesh);

	GripPoint = CreateDefaultSubobject<UVR_GrabComponent>(TEXT("GripPoint"));
	GripPoint->SetupAttachment(Mesh);
	GripPoint->SetGrabType(EGrabTypes::Snap);
}

// Called when the game starts or when spawned
void AGrabbableGun::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGrabbableGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGrabbableGun::OnFire()
{
	if(bUseProjectiles)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		FVector MuzzleLocation = MuzzlePoint->GetComponentLocation();
		FRotator MuzzleRotation = MuzzlePoint->GetComponentRotation();

		if (AVR_Projectiles* Projectile = GetWorld()->SpawnActor<AVR_Projectiles>(ProjectileRef, MuzzleLocation, MuzzleRotation, SpawnParams))
		{

		}
	}
	else 
	{
		FVector Start = MuzzlePoint->GetComponentLocation();
		FVector End = Start + MuzzlePoint->GetForwardVector() * TraceDistance;
		FHitResult Hit;
		FCollisionQueryParams TraceParams;
		TraceParams.AddIgnoredActor(this);
		if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECollisionChannel::ECC_Visibility, TraceParams))
		{
			if (Hit.GetComponent())
			{
				EndPoint = Hit.ImpactPoint;
				Hit.GetComponent()->AddImpulse(-Hit.ImpactNormal * TraceDistance);
			}
		}
		else
		{
			EndPoint = End;
		}

	}
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->PlayHapticEffect(HapticEffect, GripPoint->GetHeldByHand());
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), FireSound, MuzzlePoint->GetComponentLocation());
}

