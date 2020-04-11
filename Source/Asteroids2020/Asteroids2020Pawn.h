// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/Pawn.h"
#include "Asteroids2020Pawn.generated.h"

UCLASS(Blueprintable)
class AAsteroids2020Pawn : public APawn
{
	GENERATED_BODY()

	/* The mesh component */
	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* ShipMeshComponent;


public:
	AAsteroids2020Pawn();
	
	/** Offset from the ships location to spawn projectiles */
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite )
	FVector GunOffset;
	
	/* How fast the weapon will fire */
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
	float FireRate;

	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
	float SpawnRate;

	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
		float dx;

	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
		float dy;

	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
		float x;

	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
		float y;

	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
		float DEGTORAD;

	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
		float angle;

	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
		bool thrust;

	/* The speed our ship moves around the level */
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
	float CurrentSpeed;

	/** Sound to play each time we fire */
	UPROPERTY(Category = Audio, EditAnywhere, BlueprintReadWrite)
	class USoundBase* FireSound;

	// Begin Actor Interface
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	virtual void NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;
	// End Actor Interface

	/* Fire a shot in the specified direction */
	void FireShot(FVector FireDirection);

	void SpawnAsteroid(int numAsteroids);

	/* Handler for the fire timer expiry */
	void ShotTimerExpired();

	/* Handler for the Asteroid spawn timer expiry */
	void SpawnTimerExpired();

	// Static names for axis bindings
	static const FName ThrustBinding;
	static const FName LookRightBinding;
	static const FName FireBinding;

private:

	/* Flag to control firing  */
	uint32 bCanFire : 1;

	/* Flag to control firing  */
	uint32 bCanSpawn : 1;

	/** Handle for efficient management of ShotTimerExpired timer */
	FTimerHandle TimerHandle_ShotTimerExpired;

	/** Handle for efficient management of ShotTimerExpired timer */
	FTimerHandle TimerHandle_SpawnTimerExpired;

	/** Max forward speed */
	UPROPERTY(Category = Pitch, EditAnywhere)
		float MaxSpeed;

public:
	/** Returns ShipMeshComponent subobject **/
	FORCEINLINE class UStaticMeshComponent* GetShipMeshComponent() const { return ShipMeshComponent; }
};

