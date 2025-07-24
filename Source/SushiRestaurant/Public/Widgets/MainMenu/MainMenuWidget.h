// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

class USushiRestaurantGameInstance;
class UButton;

/**
 * Main menu with host/join/start functionality.
 */
UCLASS()
class SUSHIRESTAURANT_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:

	virtual void NativeConstruct() override;

	// Host game button
	UPROPERTY(meta = (BindWidget))
	UButton* HostButton;

	// Join game button
	UPROPERTY(meta = (BindWidget))
	UButton* JoinButton;

	// Optional: start game button (for dedicated host flow)
	UPROPERTY(meta = (BindWidgetOptional))
	UButton* StartButton;

	// Button callbacks
	UFUNCTION()
	void OnHostClicked();

	UFUNCTION()
	void OnJoinClicked();

	UFUNCTION()
	void OnStartClicked();
	
};
