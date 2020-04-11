// Fill out your copyright notice in the Description page of Project Settings.

#include "Asteroid.h"
#include "Asteroids2020Pawn.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Engine/StaticMesh.h"

// Sets default values
AAsteroid::AAsteroid()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> AsteroidMeshAsset(TEXT("/Game/TwinStick/Meshes/SM_Rock.SM_Rock"));
	// Create the mesh component
	AsteroidMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AsteroidMesh0"));
	AsteroidMesh->SetupAttachment(RootComponent);
	AsteroidMesh->BodyInstance.SetCollisionProfileName("Asteroid");
	AsteroidMesh->SetStaticMesh(AsteroidMeshAsset.Object);
	AsteroidMesh->OnComponentHit.AddDynamic(this, &AAsteroid::OnHit);		// set up a notification for when this component hits something
	RootComponent = AsteroidMesh;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	AsteroidMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("AsteroidMovement"));
	AsteroidMovement->UpdatedComponent = AsteroidMesh;
	AsteroidMovement->InitialSpeed = 200.0f;
	AsteroidMovement->MaxSpeed = 200.0f;
	AsteroidMovement->bRotationFollowsVelocity = true;
	AsteroidMovement->bShouldBounce = true;
	AsteroidMovement->ProjectileGravityScale = 0.f; // No gravity

	InitialLifeSpan = 20.f;
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AAsteroid::Tick(float Deltaseconds) {
	FVector ActorLocation = GetActorLocation();

	if (ActorLocation.X >= 2150) ActorLocation.X = -2050; if (ActorLocation.X <= -2150) ActorLocation.X = 2050;
	if (ActorLocation.Y >= 2150) ActorLocation.Y = -2050; if (ActorLocation.Y <= -2150) ActorLocation.Y = 2050;

	SetActorLocation(ActorLocation);
}

void AAsteroid::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL) && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * 20.0f, GetActorLocation());
	}

	Destroy();
}
