// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/VR_Projectiles.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
// Sets default values
AVR_Projectiles::AVR_Projectiles()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Collider = CreateDefaultSubobject<USphereComponent>(TEXT("Collider"));
	RootComponent = Collider;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Collider);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->Velocity = FVector(1200.0f, 0.0f, 0.0f);
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;
	ProjectileMovement->Bounciness = 0.3f;

	Collider->OnComponentHit.AddDynamic(this, &AVR_Projectiles::OnHit);
}

// Called when the game starts or when spawned
void AVR_Projectiles::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AVR_Projectiles::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

void AVR_Projectiles::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{


	if(OtherComponent->IsSimulatingPhysics())
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Hit"));
		HitComponent->AddImpulseAtLocation(ProjectileMovement->Velocity * 100.0f, Hit.ImpactPoint);
	}

	Destroy();
}

void AVR_Projectiles::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulse(ProjectileMovement->Velocity);
	}
	Destroy(); 
}

