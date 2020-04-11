// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "Asteroids2020Pawn.h"
#include "Asteroids2020Projectile.h"
#include "Asteroid.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine/CollisionProfile.h"
#include "Engine/StaticMesh.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"

const FName AAsteroids2020Pawn::ThrustBinding("Thrust");
const FName AAsteroids2020Pawn::LookRightBinding("LookRight");
const FName AAsteroids2020Pawn::FireBinding("Fire");

AAsteroids2020Pawn::AAsteroids2020Pawn()
{	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ShipMesh(TEXT("/Game/TwinStick/Meshes/TwinStickUFO.TwinStickUFO"));
	// Create the mesh component
	ShipMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShipMesh"));
	RootComponent = ShipMeshComponent;
	ShipMeshComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
	ShipMeshComponent->SetStaticMesh(ShipMesh.Object);
	
	// Cache our sound effect
	static ConstructorHelpers::FObjectFinder<USoundBase> FireAudio(TEXT("/Game/TwinStick/Audio/TwinStickFire.TwinStickFire"));
	FireSound = FireAudio.Object;

	// Set handling parameters
	dx = 0, dy = 0, x = 0, y = 0, angle = 0, CurrentSpeed = 0;
	DEGTORAD = 0.017453f;
	MaxSpeed = 19.f;
	thrust = false;
	// Weapon
	GunOffset = FVector(150.f, 0.f, 0.f);
	FireRate = 0.2f;
	SpawnRate = 5.0f;
	bCanFire = true;
	bCanSpawn = true;
}


void AAsteroids2020Pawn::Tick(float DeltaSeconds)
{
	if (GetInputAxisValue(LookRightBinding) > 0) {
		angle += 3;
	}
	else if(GetInputAxisValue(LookRightBinding) < 0){
		angle -= 3;
	}

	if (GetInputAxisValue(ThrustBinding)) {
		dx += cos(angle*DEGTORAD)*0.1;
		dy += sin(angle*DEGTORAD)*0.1;
	}else
	{
		dx *= 1.0;
		dy *= 1.0;
	}

	CurrentSpeed = sqrt(dx*dx + dy*dy);
	if (CurrentSpeed > MaxSpeed)
	{
		dx *= MaxSpeed / CurrentSpeed;
		dy *= MaxSpeed / CurrentSpeed;
	}

	x += dx;
	y += dy;

	if (x >= 2000) x = -1999; if (x <= -2000) x = 1999;
	if (y >= 2000) y = -1999; if (y <= -2000) y = 1999;

	SetActorLocationAndRotation(FVector(x, y, 408.0f), FQuat(FRotator(0, angle, 0)));

	// Check if fire button is being pressed
	if (GetInputAxisValue(FireBinding)) {
		FireShot(GetActorForwardVector());
	}

	SpawnAsteroid(2);
	// Call any parent class Tick implementation
	Super::Tick(DeltaSeconds);
}

void AAsteroids2020Pawn::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	// set up gameplay key bindings
	PlayerInputComponent->BindAxis(ThrustBinding);
	PlayerInputComponent->BindAxis(LookRightBinding);
	PlayerInputComponent->BindAxis(FireBinding);
}

void AAsteroids2020Pawn::FireShot(FVector FireDirection)
{
	// If it's ok to fire again
	if (bCanFire == true)
	{
			const FRotator FireRotation = FireDirection.Rotation();
			// Spawn projectile at an offset from this pawn
			const FVector SpawnLocation = GetActorLocation() + FireRotation.RotateVector(GunOffset);

			UWorld* const World = GetWorld();
			if (World != NULL)
			{
				// spawn the projectile
				World->SpawnActor<AAsteroids2020Projectile>(SpawnLocation, FireRotation);
			}

			bCanFire = false;
			World->GetTimerManager().SetTimer(TimerHandle_ShotTimerExpired, this, &AAsteroids2020Pawn::ShotTimerExpired, FireRate);

			// try and play the sound if specified
			if (FireSound != nullptr)
			{
				UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
			}

			bCanFire = false;
	}
}

void AAsteroids2020Pawn::SpawnAsteroid(int numAsteroids) {

	// If it's ok to fire again
	if (bCanSpawn == true)
	{
		for (int i = 0; i < numAsteroids; i++) {
			// Spawn projectile at an offset from this pawn
			FVector SpawnLocation;
			FRotator SpawnRotation;
			//Random number between -2000 and 2000 
			//Random number between 1 and 4
			float randNum1 = rand() % 4000 - 2000;
			float randNum2 = rand() % 4 + 1;
			SpawnRotation = FRotator(0, randNum1, randNum1);

			if (randNum2 == 1) {
				SpawnLocation = FVector(2300.0f, randNum1, 408.0f);
				
			}
			else if (randNum2 == 2) {
				SpawnLocation = FVector(-2300.0f, randNum1, 408.0f);
				
			}
			else if (randNum2 == 3) {
				SpawnLocation = FVector(randNum1, -2300.0f, 408.0f);
				
			}
			else {
				SpawnLocation = FVector(randNum1, 2300.0f, 408.0f);
				
			}


			UWorld* const World = GetWorld();
			if (World != NULL)
			{
				// spawn the projectile
				World->SpawnActor<AAsteroid>(SpawnLocation, SpawnRotation);
			}
		}

		UWorld* const World = GetWorld();
		bCanSpawn = false;
		World->GetTimerManager().SetTimer(TimerHandle_SpawnTimerExpired, this, &AAsteroids2020Pawn::SpawnTimerExpired, SpawnRate);
		bCanSpawn = false;

	}
}

void AAsteroids2020Pawn::NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	// Deflect along the surface when we collide.
	FRotator CurrentRotation = GetActorRotation();
	SetActorRotation(FQuat::Slerp(CurrentRotation.Quaternion(), HitNormal.ToOrientationQuat(), 0.025f));
}

void AAsteroids2020Pawn::ShotTimerExpired()
{
	bCanFire = true;
}

void AAsteroids2020Pawn::SpawnTimerExpired()
{
	bCanSpawn = true;
}
