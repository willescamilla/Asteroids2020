// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"
#include "Asteroids2020Pawn.h"
#include "Asteroids2020HUD.h"
#include "Asteroids2020GameMode.h"


AAsteroids2020GameMode::AAsteroids2020GameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("Class'/Script/Asteroids2020.Asteroids2020Pawn'"));
	// set default pawn class to our character class
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FClassFinder<UUserWidget> HealthBar(TEXT("/Game/TwinStick/UI/Health_UI"));
	HUDWidgetClass = HealthBar.Class;

	// use our custom HUD class
	HUDClass = AAsteroids2020HUD::StaticClass();

	// add Health Bar UI to viewport
	if (HUDWidgetClass != nullptr)
	{
		CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), HUDWidgetClass);

		if (CurrentWidget)
		{
			CurrentWidget->AddToViewport();
		}
	}
}

void AAsteroids2020GameMode::BeginPlay() {
	Super::BeginPlay();

	SetCurrentState(EGamePlayState::EPlaying);

	MyCharacter = Cast<AAsteroids2020Pawn>(UGameplayStatics::GetPlayerPawn(this, 0));
}

void AAsteroids2020GameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (MyCharacter)
	{
		if (FMath::IsNearlyZero(MyCharacter->GetHealth(), 0.001f))
		{
			SetCurrentState(EGamePlayState::EGameOver);
		}
	}
}

EGamePlayState AAsteroids2020GameMode::GetCurrentState() const
{
	return CurrentState;
}

void AAsteroids2020GameMode::SetCurrentState(EGamePlayState NewState)
{
	CurrentState = NewState;
	HandleNewState(CurrentState);
}

void AAsteroids2020GameMode::HandleNewState(EGamePlayState NewState)
{
	switch (NewState)
	{
	case EGamePlayState::EPlaying:
	{
		// do nothing
	}
	break;
	// GameOver State
	case EGamePlayState::EGameOver:
	{
		UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
	}
	break;
	// Unknown/default state
	default:
	case EGamePlayState::EUnknown:
	{
		// do nothing
	}
	break;
	}
}