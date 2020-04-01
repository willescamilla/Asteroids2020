// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "Asteroids2020GameMode.h"
#include "Asteroids2020Pawn.h"

AAsteroids2020GameMode::AAsteroids2020GameMode()
{
	// set default pawn class to our character class
	DefaultPawnClass = AAsteroids2020Pawn::StaticClass();
}

