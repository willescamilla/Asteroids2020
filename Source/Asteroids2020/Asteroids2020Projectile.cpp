// Copyright 1998-2019 Epic Games, Inc. All Rights Reserve

#include "Kismet/GameplayStatics.h"
#include "Asteroids2020Projectile.h"
#include "Asteroid.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Engine/StaticMesh.h"

AAsteroids2020Projectile::AAsteroids2020Projectile() 
{
	// Static reference to the mesh to use for the projectile
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ProjectileMeshAsset(TEXT("/Game/TwinStick/Meshes/TwinStickProjectile.TwinStickProjectile"));
	//static ConstructorHelpers::FObjectFinder<APawn> PlayerPawnObjectFinder(TEXT("/Classes_Game/Asteroids2020/Asteroids2020Pawn.Asteroids2020Pawn"));
	//MyCharacter = PlayerPawnObjectFinder.Object;
	MyCharacter = Cast<AAsteroids2020Pawn>(UGameplayStatics::GetPlayerPawn(this, 0));
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
}

void AAsteroids2020Projectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL))
	{
		//OtherComp->AddImpulseAtLocation(GetVelocity() * 20.0f, GetActorLocation());
		UWorld* const World = GetWorld();
		if (World != NULL)
		{
			AAsteroid* Actor = (AAsteroid*)OtherActor;

			if (Actor->GetIteration() == 2) {
				OtherActor->Destroy();
				MyCharacter->SetScoreChange(2);
			}
			else if (OtherActor->GetName().Compare("TP_TwinStickPawn_1") == 0) {
				MyCharacter->Die();
			}
			else {
				MyCharacter->SetScoreChange(Actor->GetIteration());
				FVector HitAsteroidScale = OtherActor->GetActorScale3D();
				FVector HitAsteroidLocation = OtherActor->GetActorLocation();
				// spawn the projectile
				FVector SmallerAsteroid1 = HitAsteroidLocation;
				SmallerAsteroid1.X += 110;
				SmallerAsteroid1.Y += 110;

				FVector SmallerAsteroid2 = HitAsteroidLocation;
				SmallerAsteroid2.X -= 110;
				SmallerAsteroid2.Y -= 110;

				AAsteroid* SpawnedActor1 = (AAsteroid*)(World->SpawnActor<AAsteroid>(SmallerAsteroid1, FRotator(0, rand() % 360, rand() % 360)));
				SpawnedActor1->SetIteration(Actor->GetIteration() + 1);
				SpawnedActor1->SetActorScale3D(HitAsteroidScale*0.5);
				UProjectileMovementComponent* SpawnedActor1Movement = (UProjectileMovementComponent*)SpawnedActor1->GetProjectileMovement();
				SpawnedActor1Movement->SetVelocityInLocalSpace(FVector(SpawnedActor1->GetVelocity().X * 5, SpawnedActor1->GetVelocity().Y * 5, 0));

				AAsteroid* SpawnedActor2 = (AAsteroid*)(World->SpawnActor<AAsteroid>(SmallerAsteroid2, FRotator(0, rand() % 360, rand() % 360)));
				SpawnedActor2->SetIteration(Actor->GetIteration() + 1);
				SpawnedActor2->SetActorScale3D(HitAsteroidScale*0.5);
				UProjectileMovementComponent* SpawnedActor2Movement = (UProjectileMovementComponent*)SpawnedActor2->GetProjectileMovement();
				SpawnedActor2Movement->SetVelocityInLocalSpace(FVector(SpawnedActor2->GetVelocity().X * 5, SpawnedActor2->GetVelocity().Y * 5, 0));

				OtherActor->Destroy();
			}
		}
		
	}

	Destroy();
}