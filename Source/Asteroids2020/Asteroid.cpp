#include "Asteroid.h"
#include "Kismet/GameplayStatics.h"
#include "Asteroids2020Pawn.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Engine/StaticMesh.h"

// Sets default values
AAsteroid::AAsteroid()
{
	//Find the pawn in our world and inintialize it
	MyCharacter = Cast<AAsteroids2020Pawn>(UGameplayStatics::GetPlayerPawn(this, 0));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> AsteroidMeshAsset(TEXT("/Game/TwinStick/Meshes/SM_Rock.SM_Rock"));
	// Create the mesh component
	AsteroidMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AsteroidMesh0"));
	AsteroidMesh->SetupAttachment(RootComponent);
	AsteroidMesh->BodyInstance.SetCollisionProfileName("BlockAllDynamic");

	//Make Asteroids overlap eachother instead of bouce off in game breaking directions
	AsteroidMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	AsteroidMesh->SetStaticMesh(AsteroidMeshAsset.Object);
	AsteroidMesh->OnComponentHit.AddDynamic(this, &AAsteroid::OnHit);		// set up a notification for when this component hits something
	RootComponent = AsteroidMesh;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	AsteroidMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("AsteroidMovement"));
	AsteroidMovement->UpdatedComponent = AsteroidMesh;
	//Big asteroids are slow and smaller asteroids keep getting faster
	AsteroidMovement->InitialSpeed = 200.0f;
	AsteroidMovement->MaxSpeed = 0;
	AsteroidMovement->bRotationFollowsVelocity = true;
	AsteroidMovement->bShouldBounce = true;
	AsteroidMovement->ProjectileGravityScale = 0.f; // No gravity
	InitialLifeSpan = 20.f;
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Initialize large asteroids to have an iteration value of 0
	iteration = 0;
}

void AAsteroid::Tick(float Deltaseconds) {
	FVector ActorLocation = GetActorLocation();

	//Bound asteroids to the play area but slightly off screen 
	// This looks more natural because of their size
	if (ActorLocation.X >= 2200) ActorLocation.X = -2100; if (ActorLocation.X <= -2200) ActorLocation.X = 2100;
	if (ActorLocation.Y >= 2200) ActorLocation.Y = -2100; if (ActorLocation.Y <= -2200) ActorLocation.Y = 2100;

	//Get rid of any asteroids that should have been destroyed already
	if (GetIteration() > 2) {
		Destroy();
	}
	SetActorLocation(ActorLocation);
}

void AAsteroid::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL) && (OtherActor == MyCharacter))
	{ 
		UWorld* const World = GetWorld();
		if (World != NULL) {
			//If we knocked into the pawn, take away one heart and then destroy self
			MyCharacter->CreateExplosion(Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
			Destroy();
			MyCharacter->Die();
		}
	}

}

void AAsteroid::SetIteration(int num) {
	iteration = num;
}
