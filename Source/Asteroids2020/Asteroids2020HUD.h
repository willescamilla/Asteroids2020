// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Asteroids2020HUD.generated.h"

/**
 * 
 */
UCLASS()
class ASTEROIDS2020_API AAsteroids2020HUD : public AHUD
{
	GENERATED_BODY()
	
public:
	AAsteroids2020HUD();

	/**Primary draw call for the HUD**/
	virtual void DrawHUD() override;

	virtual void BeginPlay() override;

private:

	UPROPERTY(EditAnywhere, Category = "Health")
		TSubclassOf<class UUserWidget> HUDWidgetClass;

	UPROPERTY(EditAnywhere, Category = "Health")
		class UUserWidget* CurrentWidget;
};
