// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EquippedVisualData.generated.h"

UENUM(BlueprintType)
enum class EEquippedVisual : uint8
{
	None   UMETA(DisplayName = "None"),
	Knife  UMETA(DisplayName = "Chef's Knife"),
	MAX    UMETA(Hidden)
};

USTRUCT(BlueprintType)
struct FEquippedVisualData : public FTableRowBase
{
	GENERATED_BODY()

	/** The Static Mesh to display when this visual is equipped. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMesh> Mesh;
};
