#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Asteroids2020Pawn.h"
#include "Asteroid.generated.h"

class UProjectileMovementComponent;
class UStaticMeshComponent;

UCLASS(config=Game)
class ASTEROIDS2020_API AAsteroid : public AActor
{
	GENERATED_BODY()

	/** Sphere collision component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* AsteroidMesh;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* AsteroidMovement;
	
public:	
	AAsteroid();

	//Variable to keep track of size of asteroid for score keeping and knowing when to stop spawning children
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
		int iteration;

	/** Function to handle the projectile hitting something */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	//Set iteration variable
	UFUNCTION()
	void SetIteration(int num);

	//Get iteration variable
	FORCEINLINE int GetIteration() { return iteration; }

	// Begin Actor Interface
	virtual void Tick(float DeltaSeconds) override;

	//Pointer to our player pawn
	AAsteroids2020Pawn* MyCharacter;

	/** Returns ProjectileMesh subobject **/
	FORCEINLINE UStaticMeshComponent* GetAsteroidMesh() const { return AsteroidMesh; }
	/** Returns ProjectileMovement subobject **/
	FORCEINLINE UProjectileMovementComponent* GetProjectileMovement() const { return AsteroidMovement; }


};
