// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/SushiRestaurantGameInstance.h"

#include "Kismet/GameplayStatics.h"

void USushiRestaurantGameInstance::HostGame() const
{
	if (!IsRunningDedicatedServer())
	{
		UE_LOG(LogTemp, Log, TEXT("Hosting game..."));
		UGameplayStatics::OpenLevel(GetWorld(), TEXT("Lvl_Restaurant"), true, TEXT("listen"));
	}
}

void USushiRestaurantGameInstance::JoinGame() const
{
	UE_LOG(LogTemp, Log, TEXT("Joining game at 127.0.0.1..."));

	if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0); PC && PC->IsLocalController())
	{
		PC->ClientTravel(TEXT("127.0.0.1"), TRAVEL_Absolute);
	}
}

void USushiRestaurantGameInstance::StartGame() const
{
	UE_LOG(LogTemp, Log, TEXT("Starting game..."));
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("Lvl_Restaurant"), true);
}
