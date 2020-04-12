// Fill out your copyright notice in the Description page of Project Settings.


#include "Asteroids2020HUD.h"
#include "Engine/Canvas.h"
#include "Engine/Texture2D.h"
#include "TextureResource.h"
#include "CanvasItem.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"

AAsteroids2020HUD::AAsteroids2020HUD()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> HealthBarObj(TEXT("/Game/TwinStick/UI/Health_UI"));
	HUDWidgetClass = HealthBarObj.Class;

}

void AAsteroids2020HUD::DrawHUD()
{
	Super::DrawHUD();
}

void AAsteroids2020HUD::BeginPlay()
{
	Super::BeginPlay();

	if (HUDWidgetClass != nullptr)
	{
		CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), HUDWidgetClass);

		if (CurrentWidget)
		{
			CurrentWidget->AddToViewport();
		}
	}
}

