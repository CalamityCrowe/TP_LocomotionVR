// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/GrabableActors.h"
#include "Components/VR_GrabComponent.h"
// Sets default values
AGrabableActors::AGrabableActors()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
	GrabbableComponent = CreateDefaultSubobject<UVR_GrabComponent>(TEXT("GrabbableComponent"));
	GrabbableComponent->SetupAttachment(Mesh);
}

// Called when the game starts or when spawned
void AGrabableActors::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGrabableActors::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

