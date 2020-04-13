
//For displaying messages to the screen for debugging
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
#include "Particles/ParticleSystem.h"
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

	//Initializing Explosion asset
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleSystemAsset(TEXT("/Game/TwinStick/Scene/P_Explosion.P_Explosion"));
	EmitterTemplate = (UParticleSystem*)ParticleSystemAsset.Object;

	// Set motion handling parameters
	dx = 0, dy = 0, x = 0, y = 0, angle = 0, CurrentSpeed = 0;
	DEGTORAD = 0.017453f;
	MaxSpeed = 19.f;
	thrust = false;

	// Weapon
	GunOffset = FVector(210.f, 0.f, 0.f);
	FireRate = 0.2f;
	bCanFire = true;

	//Asteroid spawning handlers
	SpawnRate = 8.0f;
	bCanSpawn = true;
}

void AAsteroids2020Pawn::BeginPlay() {
	//Call the base class
	Super::BeginPlay();

	/*Initialize HUD variables*/
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

	//Set up Timeline variables and related functions
	if (MagicCurve) {
		FOnTimelineFloat TimelineCallback;
		FOnTimelineEventStatic TimelineFinishedCallback;

		TimelineCallback.BindUFunction(this, FName("SetMagicValue"));
		TimelineFinishedCallback.BindUFunction(this, FName("SetMagicState"));

		MyTimeline = NewObject<UTimelineComponent>(this, FName("Magic Animation"));
		MyTimeline->AddInterpFloat(MagicCurve, TimelineCallback);
		MyTimeline->SetTimelineFinishedFunc(TimelineFinishedCallback);
		MyTimeline->RegisterComponent();
	}

	if (Theme != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, Theme, GetActorLocation());
	}
}


void AAsteroids2020Pawn::Tick(float DeltaSeconds)
{
	// Call any parent class Tick implementation
	Super::Tick(DeltaSeconds);

	//Keep track of time
	if (MyTimeline != nullptr) { MyTimeline->TickComponent(DeltaSeconds, ELevelTick::LEVELTICK_TimeOnly,nullptr); }
	
	/** This next line I created at a point where I was losing my sanity. I like to keep it here */
	//printFString("This Actor's name: %s", *this->GetName());

	/*Update Pawn movement variables*/
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

	//Make pawn wrap around the level
	if (x >= 2000) x = -1999; if (x <= -2000) x = 1999;
	if (y >= 2000) y = -1999; if (y <= -2000) y = 1999;
	//Update pawn location and rotation every tick
	SetActorLocationAndRotation(FVector(x, y, 408.0f), FQuat(FRotator(0, angle, 0)));

	// Check if fire button is being pressed
	if (GetInputAxisValue(FireBinding)) {
		FireShot(GetActorForwardVector());
	}
	//Check if Special ability button is being pressed
	if (GetInputAxisValue(SpecialAbilityBinding)) {
		UseAbility();
	}

	//Spawn 2 large asteroids every tick
	SpawnAsteroid(2);
	
}

//Try to use invincibily ability and play sound.
//Restart the timer for when the bar will fill back to full
void AAsteroids2020Pawn::UseAbility() {
	if (!FMath::IsNearlyZero(Magic, 0.001f) && bCanUseMagic)
	{
		if (AbilitySound != nullptr)
		{
			UGameplayStatics::PlaySoundAtLocation(this, AbilitySound, GetActorLocation());
		}

		MyTimeline->Stop();
		GetWorldTimerManager().ClearTimer(MagicTimerHandle);
		SetMagicChange(-25.0f);
		GetWorldTimerManager().SetTimer(MagicTimerHandle, this, &AAsteroids2020Pawn::UpdateMagic, 30.0f, false);
	}
}

//Try and Fire shot directly where the pawn is facing, play sound if successful.
//Create quick timer based on FireRate
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

//Spawn Large asteroids in random location with random rotations
//Both values have a locked Z axis so that they can stay interacting in the playable area
void AAsteroids2020Pawn::SpawnAsteroid(int numAsteroids) {

	// If it's ok to spawn again
	if (bCanSpawn == true)
	{
		for (int i = 0; i < numAsteroids; i++) {
			FVector SpawnLocation;
			FRotator SpawnRotation;
			//Random number between -2000 and 2000 
			float randNum1 = rand() % 4000 - 2000;
			//Random number between 1 and 4
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
				// spawn the asteroid
				World->SpawnActor<AAsteroid>(SpawnLocation, SpawnRotation);
			}
		}
		//Set quick timer for spawn rate of asteroids...(it's 8 seconds)
		UWorld* const World = GetWorld();
		bCanSpawn = false;
		World->GetTimerManager().SetTimer(TimerHandle_SpawnTimerExpired, this, &AAsteroids2020Pawn::SpawnTimerExpired, SpawnRate);
		bCanSpawn = false;

	}
}

//Controlling what happens if the pawn gets hit
void AAsteroids2020Pawn::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL))
	{
		//We dont want to destroy right away, we want to take one heart away
		//Set timer for next tick because there were some double hits registering
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

/*Most of these functions are for the Widget/HUD display*/
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

//Smaller asteroids yeild larger score increases
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


void AAsteroids2020Pawn::DamageTimer() {
	GetWorldTimerManager().SetTimer(MemberTimerHandle, this, &AAsteroids2020Pawn::SetDamageState, 2.0f, false);
}

//Smoothly interpolate magic meter between percent values
void AAsteroids2020Pawn::SetMagicValue() {
	TimelineValue = MyTimeline->GetPlaybackPosition();
	CurveFloatValue = PreviousMagic + MagicValue * MagicCurve->GetFloatValue(TimelineValue);
	Magic = CurveFloatValue * FullMagic;
	Magic = FMath::Clamp(Magic, 0.0f, FullMagic);
	MagicPercentage = CurveFloatValue;
	MagicPercentage = FMath::Clamp(MagicPercentage, 0.0f, 1.0f);
}

//Allow user to use magic again
void AAsteroids2020Pawn::SetMagicState() {
	bCanUseMagic = true;
	MagicValue = 0.0f;
	if (ShipDefaultMaterial) {
		ShipMeshComponent->SetMaterial(0, ShipDefaultMaterial);
	}
}

//Play animation that you just got hit
bool AAsteroids2020Pawn::PlayRedFlash() {
	if (redFlashing) {
		redFlashing = false;
		return true;
	}

	return false;
}

//Play animation that you are invincible
bool AAsteroids2020Pawn::PlayGoldFlash() {
	if (goldFlashing) {
		goldFlashing = false;
		return true;
	}

	return false;
}

//Called instead of destroying pawn
//If we can be hit, tells HUD to start flashing red, changes ship mesh to red and translucent
//Take away one heart from health
//Disallows more damage until DamageTimer() is over
void AAsteroids2020Pawn::Die() {
	if (bCanBeDamaged) {
		bCanBeDamaged = false;
		redFlashing = true;
		if (ShipRespawnMaterial) {
			ShipMeshComponent->SetMaterial(0, ShipRespawnMaterial);
		}
		UpdateHealth();
		DamageTimer();

		CreateExplosion(GetActorLocation(), GetActorRotation());
	}
}

//Allow ship to be damaged again, return mesh to normal
void AAsteroids2020Pawn::SetDamageState() {
	bCanBeDamaged = true;
	if (ShipDefaultMaterial) {
		ShipMeshComponent->SetMaterial(0, ShipDefaultMaterial);
	}
}

//Triggers the GameOver screen from Widget
bool AAsteroids2020Pawn::PlayGameOver() {
	if (gameOver) {
		Destroy();
		return true;
	}

	return false;
}

//Decrease helth by 1
//Update Health Percentage on screen
//Check if we are out of lives
void AAsteroids2020Pawn::UpdateHealth() {
	Health -= 1.0f;
	HealthPercentage = Health/FullHealth;

	if (Health <= 0.0f) {
		gameOver = true;
	}
}

void AAsteroids2020Pawn::UpdateMagic() {
	PreviousMagic = MagicPercentage;
	MagicPercentage = Magic / FullMagic;
	MagicValue = 1.0f;
	MyTimeline->PlayFromStart();
}

//Called by UseAbility()
//Changes value of magic bar
//Updates booleans to prevent damage and prevent more magic usage
//Starts both timers over again
void AAsteroids2020Pawn::SetMagicChange(float MagicChange) {
	bCanUseMagic = false;
	bCanBeDamaged = false;
	goldFlashing = true;
	PreviousMagic = MagicPercentage;
	MagicValue = MagicChange / FullMagic;
	if (ShipRespawnMaterial) {
		ShipMeshComponent->SetMaterial(0, ShipAbilityMaterial);
	}
	DamageTimer();
	MyTimeline->PlayFromStart();
}

//Called from other C++ classes as well as this one
//Plays explosion animation
void AAsteroids2020Pawn::CreateExplosion(FVector Location, FRotator Rotation) {
	UWorld* const World = GetWorld();
	UGameplayStatics::SpawnEmitterAtLocation(World, EmitterTemplate, Location, Rotation);

	// try and play the sound if specified
	if (ExplosionSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation());
	}
}