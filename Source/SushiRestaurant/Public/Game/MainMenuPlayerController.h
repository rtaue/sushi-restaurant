// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainMenuPlayerController.generated.h"

class UMainMenuWidget;

/**
 * PlayerController used in the main menu.
 * Responsible for activating mouse and input for UI and creating the main menu.
 */
UCLASS()
class SUSHIRESTAURANT_API AMainMenuPlayerController : public APlayerController
{
	GENERATED_BODY()

	public:

	AMainMenuPlayerController();

protected:
	virtual void BeginPlay() override;

	// Widget class to instantiate as the main menu
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UMainMenuWidget> MainMenuWidgetClass;

private:
	UPROPERTY()
	UMainMenuWidget* MainMenuWidget;
};
