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
	//Override the parent class tick function
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

public:
	AAsteroids2020Pawn();
	/** Sound variables */
	UPROPERTY(Category = Audio, EditAnywhere, BlueprintReadWrite)
		class USoundBase* FireSound;

	UPROPERTY(Category = Audio, EditAnywhere, BlueprintReadWrite)
		class USoundBase* Theme;

	UPROPERTY(Category = Audio, EditAnywhere, BlueprintReadWrite)
		class USoundBase* AbilitySound;

	UPROPERTY(Category = Audio, EditAnywhere, BlueprintReadWrite)
		class USoundBase* ExplosionSound;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	// End Actor Interface

	UFUNCTION()
	void CreateExplosion(FVector Location, FRotator Rotation);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Projectile, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* EmitterTemplate;

	/* Fire a shot in the specified direction */
	void FireShot(FVector FireDirection);

	//Spawn specified number of asteroids
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

	/*Pawn Movement variables*/
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

	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
		float CurrentSpeed;

	/*Variables for HUD/Widget elements*/

	UPROPERTY(Category = "Health", EditAnywhere, BlueprintReadWrite)
		float FullHealth;

	UPROPERTY(Category = "Health", EditAnywhere, BlueprintReadWrite)
		float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		float HealthPercentage;

	UPROPERTY(Category = "Health", EditAnywhere, BlueprintReadWrite)
		bool redFlashing;

	UPROPERTY(Category = "Health", EditAnywhere, BlueprintReadWrite)
		bool gameOver;

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

	UTimelineComponent* MyTimeline;
	struct FTimerHandle MemberTimerHandle;
	struct FTimerHandle MagicTimerHandle;

	float CurveFloatValue;
	float TimelineValue;
	bool bCanUseMagic;

	/*Functions for HUD elements*/
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

	UFUNCTION(BlueprintPure, Category = "Health")
		bool PlayGameOver();

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

	/* Flag to control spawning  */
	uint32 bCanSpawn : 1;

	/** Handle for efficient management of ShotTimerExpired timer */
	FTimerHandle TimerHandle_ShotTimerExpired;

	/** Handle for efficient management of SpawnTimerExpired timer */
	FTimerHandle TimerHandle_SpawnTimerExpired;

	/** Max speed */
	UPROPERTY(Category = Pitch, EditAnywhere)
		float MaxSpeed;

public:
	/** Returns ShipMeshComponent subobject **/
	FORCEINLINE class UStaticMeshComponent* GetShipMeshComponent() const { return ShipMeshComponent; }
};

