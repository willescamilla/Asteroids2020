#define printFString(text, fstring) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Magenta, FString::Printf(TEXT(text), fstring))

#include "TriggerVolumeY.h"
#include "Engine/Engine.h"
#include "Engine/Public/TimerManager.h"
// include draw debug helpers header file
#include "DrawDebugHelpers.h"

ATriggerVolumeY::ATriggerVolumeY()
{
	//Register Events
	OnActorBeginOverlap.AddDynamic(this, &ATriggerVolumeY::OnOverlapBegin);
	OnActorEndOverlap.AddDynamic(this, &ATriggerVolumeY::OnOverlapEnd);
}

// Called when the game starts or when spawned
void ATriggerVolumeY::BeginPlay()
{
	Super::BeginPlay();

	DrawDebugBox(GetWorld(), GetActorLocation(), GetComponentsBoundingBox().GetExtent(), FColor::Cyan, true, -1, 0, 5);

}

void ATriggerVolumeY::OnOverlapBegin(class AActor* OverlappedActor, class AActor* OtherActor)
{
	if (OtherActor && (OtherActor != this)) {
		// Teleport actor to other side of playable area
		// Delay function by one tick to avoid infinite back and forth teleport
		// Use -0.988 instead of -1.0 to further avoid infinite teleport
		GetWorldTimerManager().SetTimerForNextTick([OtherActor, this]() { OtherActor->SetActorLocation(OtherActor->GetActorLocation() * FVector(1.0f, -0.988f, 1.0f), false, 0, ETeleportType::None); });

	}
}

void ATriggerVolumeY::OnOverlapEnd(class AActor* OverlappedActor, class AActor* OtherActor)
{
	if (OtherActor && (OtherActor != this))
	{
		
	}
}