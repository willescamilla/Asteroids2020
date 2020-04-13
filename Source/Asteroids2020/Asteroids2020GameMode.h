#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Asteroids2020Pawn.h"
#include "Asteroids2020GameMode.generated.h"

//Determine 3 different game states
UENUM()
enum class EGamePlayState
{
	EPlaying,
	EGameOver,
	EUnknown
};

UCLASS(MinimalAPI)
class AAsteroids2020GameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AAsteroids2020GameMode();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	AAsteroids2020Pawn* MyCharacter;

	/*Returns the current playing state*/
	UFUNCTION(BlueprintPure, Category = "Health")
	EGamePlayState GetCurrentState() const;

	/*Sets a new playing state */
	void SetCurrentState(EGamePlayState NewState);

	UPROPERTY(EditAnywhere, Category = "Health")
		TSubclassOf<class UUserWidget> HUDWidgetClass;

	UPROPERTY(EditAnywhere, Category = "Health")
		class UUserWidget* CurrentWidget;

private:
	/*Keeps track of the current playing state */
	EGamePlayState CurrentState;

	/*Handle any function calls that rely upon changing the playing state of our game */
	void HandleNewState(EGamePlayState NewState);
};



