// Copyright Epic Games, Inc. All Rights Reserved.

#include "Game/SushiRestaurantGameMode.h"

#include "Characters/SushiRestaurantCharacter.h"
#include "Controllers/SushiRestaurantPlayerController.h"
#include "Game/SushiRestaurantGameState.h"
#include "Game/SushiRestaurantHUD.h"
#include "Game/SushiRestaurantPlayerState.h"

ASushiRestaurantGameMode::ASushiRestaurantGameMode()
{
	PlayerControllerClass = ASushiRestaurantPlayerController::StaticClass();
	GameStateClass = ASushiRestaurantGameState::StaticClass();
	PlayerStateClass = ASushiRestaurantPlayerState::StaticClass();
	DefaultPawnClass = ASushiRestaurantCharacter::StaticClass();
	HUDClass = ASushiRestaurantHUD::StaticClass();
}
