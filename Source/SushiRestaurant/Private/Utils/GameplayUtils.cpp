// Fill out your copyright notice in the Description page of Project Settings.


#include "Utils/GameplayUtils.h"

#include "Kismet/GameplayStatics.h"

bool UGameplayUtils::IsLocallyControlled(const UWorld* World)
{
	if (!World) return false;

	const APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
	return PC && PC->IsLocalController();
}
