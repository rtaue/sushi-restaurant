// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/MainMenu/MainMenuWidget.h"

#include "Components/Button.h"
#include "Game/SushiRestaurantGameInstance.h"
#include "Kismet/GameplayStatics.h"

void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (HostButton)
	{
		HostButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnHostClicked);
	}

	if (JoinButton)
	{
		JoinButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnJoinClicked);
	}

	if (StartButton)
	{
		StartButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnStartClicked);
	}
}

void UMainMenuWidget::OnHostClicked()
{
	if (const USushiRestaurantGameInstance* SushiGI = Cast<USushiRestaurantGameInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		SushiGI->HostGame();
	}
}

void UMainMenuWidget::OnJoinClicked()
{
	if (const USushiRestaurantGameInstance* SushiGI = Cast<USushiRestaurantGameInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		SushiGI->JoinGame();
	}
}

void UMainMenuWidget::OnStartClicked()
{
	if (const USushiRestaurantGameInstance* SushiGI = Cast<USushiRestaurantGameInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		SushiGI->StartGame();;
	}
}
