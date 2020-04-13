#include "Kismet/GameplayStatics.h"
#include "Asteroids2020Projectile.h"
#include "Asteroid.h"
#include "UObject/ConstructorHelpers.h"
#include "Particles/ParticleSystem.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Engine/StaticMesh.h"

AAsteroids2020Projectile::AAsteroids2020Projectile() 
{
	// Static reference to the mesh to use for the projectile
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ProjectileMeshAsset(TEXT("/Game/TwinStick/Meshes/TwinStickProjectile.TwinStickProjectile"));
	
	// Create mesh component for the projectile sphere
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh0"));
	ProjectileMesh->SetStaticMesh(ProjectileMeshAsset.Object);
	ProjectileMesh->SetupAttachment(RootComponent);
	ProjectileMesh->BodyInstance.SetCollisionProfileName("Projectile");
	ProjectileMesh->OnComponentHit.AddDynamic(this, &AAsteroids2020Projectile::OnHit);		// set up a notification for when this component hits something
	RootComponent = ProjectileMesh;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement0"));
	ProjectileMovement->UpdatedComponent = ProjectileMesh;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.f; // No gravity

	// Die after 1.22 seconds by default
	InitialLifeSpan = 1.22f;

	MyCharacter = Cast<AAsteroids2020Pawn>(UGameplayStatics::GetPlayerPawn(this, 0));
}

void AAsteroids2020Projectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL))
	{
		UWorld* const World = GetWorld();
		if (World != NULL)
		{
			AAsteroid* Actor = (AAsteroid*)OtherActor;

			//If we hit the pawn, take away a heart, dont destroy like asteroids
			if (OtherActor->GetName().Compare("TP_TwinStickPawn_1") == 0) {
				MyCharacter->CreateExplosion(Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
				MyCharacter->Die();
			}//If we hit a small size asteroid, destroy it and send the score to the pawn without spaning new asteroids
			else if (Actor->GetIteration() >= 2) {
				MyCharacter->CreateExplosion(Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
				OtherActor->Destroy();
				MyCharacter->SetScoreChange(2);
			}
			else {
				/*Update score because we hit a large or medium asteroid and spawn two new smaller asteroids at an offset of the parent*/
				MyCharacter->SetScoreChange(Actor->GetIteration());
				FVector HitAsteroidScale = OtherActor->GetActorScale3D();
				FVector HitAsteroidLocation = OtherActor->GetActorLocation();

				//Create random rotation for new asteroids, leave pitch untouched to keep on same Z plane
				FRotator randomRotation = FRotator(0, rand() % 360, rand() % 360);

				//Spawn the children
				SpawnAsteroidChildren(World, Actor, HitAsteroidScale, HitAsteroidLocation, randomRotation);

				//Trigger explosion and destroy parent asteroid
				MyCharacter->CreateExplosion(Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
				OtherActor->Destroy();
			}
		}
		
	}

	//Destroy projectile too
	Destroy();
}

void AAsteroids2020Projectile::SpawnAsteroidChildren(UWorld* const World, AAsteroid* Actor, FVector HitAsteroidScale, FVector HitAsteroidLocation, FRotator randomRotation) {
	//Integer for handling the difference between first child and second child
	int num = 1;
	for (int i = 0; i < 2; i++) {

		//Create X,Y location offset... leave Z untouched
		FVector SmallerAsteroid1 = HitAsteroidLocation;
		SmallerAsteroid1.X += 110*num;
		SmallerAsteroid1.Y += 110*num;

		//Create pointer to newly spawned child
		AAsteroid* SpawnedActor1 = (AAsteroid*)(World->SpawnActor<AAsteroid>(SmallerAsteroid1, randomRotation*num));

		//Update "Iteration" of spawned asteroid indicating what size the asteroid is
		SpawnedActor1->SetIteration(Actor->GetIteration() + 1);

		//Make the child half the size of parent asteroid
		SpawnedActor1->SetActorScale3D(HitAsteroidScale*0.5);

		//Create a pointer to the movement component of the spawned asteroid and speed up X and Y velocities
		UProjectileMovementComponent* SpawnedActor1Movement = (UProjectileMovementComponent*)SpawnedActor1->GetProjectileMovement();
		SpawnedActor1Movement->SetVelocityInLocalSpace(FVector(SpawnedActor1->GetVelocity().X * 5, SpawnedActor1->GetVelocity().Y * 5, 0));

		//Flip num
		num = -1;
	}
}