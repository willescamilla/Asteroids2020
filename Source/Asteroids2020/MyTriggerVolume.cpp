#define printFString(text, fstring) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Magenta, FString::Printf(TEXT(text), fstring))

#include "MyTriggerVolume.h"
#include "Engine/Engine.h"
// include draw debug helpers header file
#include "DrawDebugHelpers.h"

AMyTriggerVolume::AMyTriggerVolume()
{
	//Register Events
	OnActorBeginOverlap.AddDynamic(this, &AMyTriggerVolume::OnOverlapBegin);
	OnActorEndOverlap.AddDynamic(this, &AMyTriggerVolume::OnOverlapEnd);
}

// Called when the game starts or when spawned
void AMyTriggerVolume::BeginPlay()
{
	Super::BeginPlay();

	DrawDebugBox(GetWorld(), GetActorLocation(), GetComponentsBoundingBox().GetExtent(), FColor::Cyan, true, -1, 0, 5);

}

void AMyTriggerVolume::OnOverlapBegin(class AActor* OverlappedActor, class AActor* OtherActor)
{
	if (OtherActor && (OtherActor != this)) {
		// print to screen using above defined method when actor enters trigger volume
		printFString("%s has entered the box", *OtherActor->GetName());

		MyProjectile = Cast<AAsteroids2020Projectile>(OtherActor);

		if (MyProjectile)
		{
			printFString("%s Projectile Overlap Successful", *OtherActor->GetName());
		}
		
	}
}

void AMyTriggerVolume::OnOverlapEnd(class AActor* OverlappedActor, class AActor* OtherActor)
{
	if (OtherActor && (OtherActor != this))
	{
		// print to screen using above defined method when actor leaves trigger volume
		printFString("%s has left the Trigger Volume", *OtherActor->GetName());
	}
}