// Fill out your copyright notice in the Description page of Project Settings.


#include "Utils/GameplayUtils.h"

#include "Kismet/GameplayStatics.h"

bool UGameplayUtils::IsLocallyControlled(const UWorld* World)
{
	if (!World) return false;

	const APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
	return PC && PC->IsLocalController();
}

void UGameplayUtils::UpdateWidgetFacing(USceneComponent* TargetWidget, UObject* WorldContext)
{
	if (!TargetWidget || !WorldContext) return;

	if (const UWorld* World = WorldContext->GetWorld(); !World || World->IsNetMode(NM_DedicatedServer)) return;

	// Get player camera
	if (const APlayerCameraManager* CamManager = UGameplayStatics::GetPlayerCameraManager(WorldContext, 0))
	{
		const FVector CamLoc = CamManager->GetCameraLocation();
		const FVector WidgetLoc = TargetWidget->GetComponentLocation();

		FRotator LookAtRotation = (CamLoc - WidgetLoc).Rotation();
		LookAtRotation.Pitch = 0.f;
		LookAtRotation.Roll = 0.f;

		TargetWidget->SetWorldRotation(LookAtRotation);
	}
}
