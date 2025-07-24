// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/MainMenuPlayerController.h"

#include "Widgets/MainMenu/MainMenuWidget.h"

AMainMenuPlayerController::AMainMenuPlayerController(): MainMenuWidget(nullptr)
{
	static ConstructorHelpers::FClassFinder<UMainMenuWidget> WidgetClass(TEXT("/Game/UI/MainMenu/WBP_MainMenu"));

	if (WidgetClass.Succeeded())
	{
		MainMenuWidgetClass = WidgetClass.Class;
	}
}

void AMainMenuPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Enable mouse and UI interaction
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
	SetInputMode(FInputModeUIOnly());

	// Create and add the main menu widget to the viewport
	if (MainMenuWidgetClass && IsLocalController())
	{
		MainMenuWidget = CreateWidget<UMainMenuWidget>(this, MainMenuWidgetClass);
		if (MainMenuWidget)
		{
			MainMenuWidget->AddToViewport();
		}
	}
}
