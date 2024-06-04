// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/DefaultPawn.h"
#include "My_Player.h"
#include "MyGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class BLUE_API AMyGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	void InitGameState() override;

private:
	UPROPERTY(EditAnywhere, NoClear)
	TSubclassOf<AMy_Player> CustomXPCharacterClass = AMy_Player::StaticClass();
};
