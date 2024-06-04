// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameModeBase.h"

void  AMyGameModeBase::InitGameState()
{
    Super::InitGameState();
    // Check if no hard override is active
    if (DefaultPawnClass == ADefaultPawn::StaticClass() || !DefaultPawnClass)
    {
        // Update to custom implementation
        DefaultPawnClass = CustomXPCharacterClass;
    }
}