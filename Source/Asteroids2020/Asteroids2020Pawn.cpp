// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
#define printFString(text, fstring) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Magenta, FString::Printf(TEXT(text), fstring))

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
#include "Engine/Engine.h"
#include "Engine/StaticMesh.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Sound/SoundBase.h"

const FName AAsteroids2020Pawn::ThrustBinding("Thrust");
const FName AAsteroids2020Pawn::LookRightBinding("LookRight");
const FName AAsteroids2020Pawn::FireBinding("Fire");
const FName AAsteroids2020Pawn::SpecialAbilityBinding("SpecialAbility");

AAsteroids2020Pawn::AAsteroids2020Pawn()
{
	// Create the mesh component
	ShipMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShipMesh"));
	ShipMeshComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
	ShipMeshComponent->OnComponentHit.AddDynamic(this, &AAsteroids2020Pawn::OnHit);
	RootComponent = ShipMeshComponent;


	// Set handling parameters
	dx = 0, dy = 0, x = 0, y = 0, angle = 0, CurrentSpeed = 0;
	DEGTORAD = 0.017453f;
	MaxSpeed = 19.f;
	thrust = false;
	// Weapon
	GunOffset = FVector(210.f, 0.f, 0.f);
	FireRate = 0.2f;
	SpawnRate = 8.0f;
	bCanFire = true;
	bCanSpawn = true;
}

void AAsteroids2020Pawn::BeginPlay() {
	//Call the base class
	Super::BeginPlay();

	ScoreNumber = 0;
	BigValue = 20;
	MediumValue = 50;
	SmallValue = 100;

	FullHealth = 3.0f;
	Health = FullHealth;
	HealthPercentage = 1.0f;
	bCanBeDamaged = true;

	FullMagic = 100.0f;
	Magic = FullMagic;
	MagicPercentage = 1.0f;
	PreviousMagic = MagicPercentage;
	MagicValue = 0.0f;
	bCanUseMagic = true;

	if (MagicCurve) {
		FOnTimelineFloat TimelineCallback;
		FOnTimelineEventStatic TimelineFinishedCallback;

		TimelineCallback.BindUFunction(this, FName("SetMagicValue"));
		TimelineFinishedCallback.BindUFunction(this, FName("SetMagicState"));
		MyTimeline.AddInterpFloat(MagicCurve, TimelineCallback);
		MyTimeline.SetTimelineFinishedFunc(TimelineFinishedCallback);
	}
}


void AAsteroids2020Pawn::Tick(float DeltaSeconds)
{
	// Call any parent class Tick implementation
	Super::Tick(DeltaSeconds);

	MyTimeline.TickTimeline(DeltaSeconds);

	//printFString("This Actor's name: %s", *this->GetName());

	if (GetInputAxisValue(LookRightBinding) > 0) {
		angle += 3;
	}
	else if (GetInputAxisValue(LookRightBinding) < 0) {
		angle -= 3;
	}

	if (GetInputAxisValue(ThrustBinding)) {
		dx += cos(angle*DEGTORAD)*0.1;
		dy += sin(angle*DEGTORAD)*0.1;
	}
	else
	{
		dx *= 1.0;
		dy *= 1.0;
	}

	CurrentSpeed = sqrt(dx*dx + dy * dy);
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

	if (GetInputAxisValue(SpecialAbilityBinding)) {
		UseAbility();
	}

	SpawnAsteroid(2);
	
}

void AAsteroids2020Pawn::UseAbility() {
	if (!FMath::IsNearlyZero(Magic, 0.001f) && bCanUseMagic)
	{
		if (AbilitySound != nullptr)
		{
			UGameplayStatics::PlaySoundAtLocation(this, AbilitySound, GetActorLocation());
		}

		MyTimeline.Stop();
		GetWorldTimerManager().ClearTimer(MagicTimerHandle);
		SetMagicChange(-20.0f);
		GetWorldTimerManager().SetTimer(MagicTimerHandle, this, &AAsteroids2020Pawn::UpdateMagic, 5.0f, false);
	}
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
				SpawnLocation = FVector(2300.0f, randNum1, 320.0f);

			}
			else if (randNum2 == 2) {
				SpawnLocation = FVector(-2300.0f, randNum1, 320.0f);

			}
			else if (randNum2 == 3) {
				SpawnLocation = FVector(randNum1, -2300.0f, 320.0f);

			}
			else {
				SpawnLocation = FVector(randNum1, 2300.0f, 320.0f);

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

void AAsteroids2020Pawn::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL))
	{
		GetWorldTimerManager().SetTimerForNextTick([OtherActor, this]() { Die(); });
	}

}

void AAsteroids2020Pawn::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	// set up gameplay key bindings
	PlayerInputComponent->BindAxis(ThrustBinding);
	PlayerInputComponent->BindAxis(LookRightBinding);
	PlayerInputComponent->BindAxis(FireBinding);
	PlayerInputComponent->BindAxis(SpecialAbilityBinding);
}

void AAsteroids2020Pawn::ShotTimerExpired()
{
	bCanFire = true;
}

void AAsteroids2020Pawn::SpawnTimerExpired()
{
	bCanSpawn = true;
}

float AAsteroids2020Pawn::GetHealth() {
	return HealthPercentage;
}

float AAsteroids2020Pawn::GetMagic() {
	return MagicPercentage;
}

FText AAsteroids2020Pawn::GetHealthIntText() {
	int32 HP = FMath::RoundHalfFromZero(HealthPercentage * 100);
	FString HPS = FString::FromInt(HP);
	FString HealthHUD = HPS + FString(TEXT("%"));
	FText HPText = FText::FromString(HealthHUD);
	return HPText;
}

int AAsteroids2020Pawn::GetScore() {
	return ScoreNumber;
}

void AAsteroids2020Pawn::SetScoreChange(int iterationNum) {

	if (iterationNum == 2) {
		ScoreNumber += SmallValue;
	}
	if (iterationNum == 1) {
		ScoreNumber += MediumValue;
	}
	else {
		ScoreNumber += BigValue;
	}
	
}

FText AAsteroids2020Pawn::GetScoreIntText() {
	FString Score = FString::FromInt(ScoreNumber);
	FString ScoreHUD = FString(TEXT("Score: ")+ Score);
	FText SCORETEXT = FText::FromString(ScoreHUD);
	return SCORETEXT;
}

FText AAsteroids2020Pawn::GetMagicIntText() {
	int32 MP = FMath::RoundHalfFromZero(MagicPercentage * 100);
	FString MPS = FString::FromInt(MP);
	FString FullMPS = FString::FromInt(FullMagic);
	FString MagicHUD = MPS + FString(TEXT("/") + FullMPS);
	FText MAGICTEXT = FText::FromString(MagicHUD);
	return MAGICTEXT;
}

void AAsteroids2020Pawn::SetDamageState() {
	bCanBeDamaged = true;
	if (ShipDefaultMaterial) {
		ShipMeshComponent->SetMaterial(0, ShipDefaultMaterial);
	}
}

void AAsteroids2020Pawn::DamageTimer() {
	GetWorldTimerManager().SetTimer(MemberTimerHandle, this, &AAsteroids2020Pawn::SetDamageState, 2.0f, false);
}

void AAsteroids2020Pawn::SetMagicValue() {
	TimelineValue = MyTimeline.GetPlaybackPosition();
	CurveFloatValue = PreviousMagic + MagicValue * MagicCurve->GetFloatValue(TimelineValue);
	Magic = CurveFloatValue * FullMagic;
	Magic = FMath::Clamp(Magic, 0.0f, FullMagic);
	MagicPercentage = CurveFloatValue;
	MagicPercentage = FMath::Clamp(MagicPercentage, 0.0f, 1.0f);
}

void AAsteroids2020Pawn::SetMagicState() {
	bCanUseMagic = true;
	MagicValue = 0.0f;
	if (ShipDefaultMaterial) {
		ShipMeshComponent->SetMaterial(0, ShipDefaultMaterial);
	}
}

bool AAsteroids2020Pawn::PlayRedFlash() {
	if (redFlashing) {
		redFlashing = false;
		return true;
	}

	return false;
}

bool AAsteroids2020Pawn::PlayGoldFlash() {
	if (goldFlashing) {
		goldFlashing = false;
		return true;
	}

	return false;
}

void AAsteroids2020Pawn::Die() {
	if (bCanBeDamaged) {
		bCanBeDamaged = false;
		redFlashing = true;
		if (ShipRespawnMaterial) {
			ShipMeshComponent->SetMaterial(0, ShipRespawnMaterial);
		}
		UpdateHealth();
		DamageTimer();
	}
}

void AAsteroids2020Pawn::UpdateHealth() {
	Health -= 1.0f;
	HealthPercentage = Health/FullHealth;
}

void AAsteroids2020Pawn::UpdateMagic() {
	PreviousMagic = MagicPercentage;
	MagicPercentage = Magic / FullMagic;
	MagicValue = 1.0f;
	MyTimeline.PlayFromStart();
}

void AAsteroids2020Pawn::SetMagicChange(float MagicChange) {
	bCanUseMagic = false;
	bCanBeDamaged = false;
	goldFlashing = true;
	PreviousMagic = MagicPercentage;
	MagicValue = MagicChange / FullMagic;
	if (ShipRespawnMaterial) {
		ShipMeshComponent->SetMaterial(0, ShipAbilityMaterial);
	}

	MyTimeline.PlayFromStart();
	DamageTimer();
}