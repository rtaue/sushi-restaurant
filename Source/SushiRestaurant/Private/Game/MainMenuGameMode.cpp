// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/MainMenuGameMode.h"

#include "Game/MainMenuHUD.h"
#include "Game/MainMenuPlayerController.h"

void AMainMenuGameMode::BeginPlay()
{
	Super::BeginPlay();

	DefaultPawnClass = nullptr;
	HUDClass = AMainMenuHUD::StaticClass();
	PlayerControllerClass = AMainMenuPlayerController::StaticClass();
}
