// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/Pawn.h"
#include "Components/TimelineComponent.h"
#include "Asteroids2020Pawn.generated.h"

UCLASS(Blueprintable)
class AAsteroids2020Pawn : public APawn
{
	GENERATED_BODY()

	/* The mesh component */
	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* ShipMeshComponent;

protected:
	// Begin Actor Interface
	virtual void BeginPlay();
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

public:
	AAsteroids2020Pawn();
	/** Sound to play each time we fire */
	UPROPERTY(Category = Audio, EditAnywhere, BlueprintReadWrite)
		class USoundBase* FireSound;

	UPROPERTY(Category = Audio, EditAnywhere, BlueprintReadWrite)
		class USoundBase* AbilitySound;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
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
	static const FName SpecialAbilityBinding;

	/** Offset from the ships location to spawn projectiles */
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
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

	UPROPERTY(Category = "Health", EditAnywhere, BlueprintReadWrite)
		float FullHealth;

	UPROPERTY(Category = "Health", EditAnywhere, BlueprintReadWrite)
		float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		float HealthPercentage;

	UPROPERTY(Category = "Health", EditAnywhere, BlueprintReadWrite)
		bool redFlashing;

	UPROPERTY(Category = "Magic", EditAnywhere, BlueprintReadWrite)
		bool goldFlashing;

	UPROPERTY(Category = "Score", EditAnywhere, BlueprintReadWrite)
		int ScoreNumber;

	UPROPERTY(Category = "Score", EditAnywhere, BlueprintReadWrite)
		int BigValue;

	UPROPERTY(Category = "Score", EditAnywhere, BlueprintReadWrite)
		int MediumValue;

	UPROPERTY(Category = "Score", EditAnywhere, BlueprintReadWrite)
		int SmallValue;

	UPROPERTY(Category = "Magic", EditAnywhere, BlueprintReadWrite)
		float FullMagic;

	UPROPERTY(Category = "Magic", EditAnywhere, BlueprintReadWrite)
		float Magic;

	UPROPERTY(Category = "Magic", EditAnywhere, BlueprintReadWrite)
		float MagicPercentage;

	UPROPERTY(Category = "Magic", EditAnywhere, BlueprintReadWrite)
		float PreviousMagic;

	UPROPERTY(Category = "Magic", EditAnywhere, BlueprintReadWrite)
		float MagicValue;

	UPROPERTY(Category = "Magic", EditAnywhere, BlueprintReadWrite)
		UCurveFloat *MagicCurve;

	UPROPERTY(Category = "Magic", EditAnywhere)
		FTimeline MyTimeline;

	UPROPERTY(Category = "Magic", EditAnywhere)
		FTimerHandle MemberTimerHandle;

	UPROPERTY(Category = "Magic", EditAnywhere)
		FTimerHandle MagicTimerHandle;

	float CurveFloatValue;
	float TimelineValue;
	bool bCanUseMagic;

	UFUNCTION(BlueprintPure, Category = "Health")
		float GetHealth();

	UFUNCTION(BlueprintPure, Category = "Health")
		FText GetHealthIntText();

	UFUNCTION(BlueprintPure, Category = "Score")
		int GetScore();

	UFUNCTION(BlueprintPure, Category = "Score")
		FText GetScoreIntText();

	UFUNCTION(BlueprintPure, Category = "Magic")
		float GetMagic();

	UFUNCTION(BlueprintPure, Category = "Magic")
		FText GetMagicIntText();

	UFUNCTION()
		void DamageTimer();

	UFUNCTION()
		void SetDamageState();

	UFUNCTION()
		void SetMagicValue();

	UFUNCTION()
		void SetMagicState();

	UFUNCTION()
		void SetMagicChange(float MagicChange);

	UFUNCTION()
		void UpdateMagic();

	UFUNCTION(BlueprintPure, Category = "Health")
		bool PlayRedFlash();

	UFUNCTION(BlueprintPure, Category = "Magic")
		bool PlayGoldFlash();

	UFUNCTION()
		void SetScoreChange(int iterationNum);

	UPROPERTY(Category = "Health", EditAnywhere)
		class UMaterialInterface* ShipDefaultMaterial;

	UPROPERTY(Category = "Health", EditAnywhere)
		class UMaterialInterface* ShipRespawnMaterial;

	UPROPERTY(Category = "Magic", EditAnywhere)
		class UMaterialInterface* ShipAbilityMaterial;
	UFUNCTION()
		void UseAbility();

	UFUNCTION()
		void Die();

	UFUNCTION(BlueprintCallable, Category = "Health")
		void UpdateHealth();

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

