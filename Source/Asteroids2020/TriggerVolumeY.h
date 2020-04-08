#pragma once

#include "Engine/TriggerVolume.h"
#include "Asteroids2020Projectile.h"
#include "TriggerVolumeY.generated.h"


UCLASS()
class ASTEROIDS2020_API ATriggerVolumeY : public ATriggerVolume
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	// constructor sets default values for this actor's properties
	ATriggerVolumeY();

	// overlap begin function
	UFUNCTION()
		void OnOverlapBegin(class AActor* OverlappedActor, class AActor* OtherActor);

	// overlap end function
	UFUNCTION()
		void OnOverlapEnd(class AActor* OverlappedActor, class AActor* OtherActor);

	// specific actor for overlap
	UPROPERTY(EditAnywhere)
		class AActor* SpecificActor;

	UPROPERTY(EditAnywhere)
		class AAsteroids2020Projectile* MyProjectile;
};
