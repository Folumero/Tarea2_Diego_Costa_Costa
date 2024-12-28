// Copyright Epic Games, Inc. All Rights Reserved.

#include "Tarea2DiegoPrograGameMode.h"
#include "Tarea2DiegoPrograCharacter.h"
#include "UObject/ConstructorHelpers.h"

ATarea2DiegoPrograGameMode::ATarea2DiegoPrograGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
