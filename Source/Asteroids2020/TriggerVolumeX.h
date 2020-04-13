#pragma once

#include "Engine/TriggerVolume.h"
#include "Asteroids2020Projectile.h"
#include "Asteroid.h"
#include "TriggerVolumeX.generated.h"


UCLASS()
class ASTEROIDS2020_API ATriggerVolumeX : public ATriggerVolume
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	// constructor sets default values for this actor's properties
	ATriggerVolumeX();

	// overlap begin function
	UFUNCTION()
		void OnOverlapBegin(class AActor* OverlappedActor, class AActor* OtherActor);

	// overlap end function
	UFUNCTION()
		void OnOverlapEnd(class AActor* OverlappedActor, class AActor* OtherActor);


	// specific actor for overlap
	UPROPERTY(EditAnywhere)
		class AActor* SpecificActor;

	/** Since projectile is spawned during gameplay,
	this is a work around to get it noticed by the volume */
	UPROPERTY(EditAnywhere)
		class AAsteroids2020Projectile* MyProjectile;

};
