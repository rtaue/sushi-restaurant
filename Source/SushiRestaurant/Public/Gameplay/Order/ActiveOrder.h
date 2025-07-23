// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/Recipe/RecipeData.h"
#include "Interfaces/RecipeSourceInterface.h"
#include "ActiveOrder.generated.h"

// Represents the possible status for an order
UENUM(BlueprintType)
enum class EOrderStatus : uint8
{
    None = 0        UMETA(DisplayName = "None"),
    Pending         UMETA(DisplayName = "Pending"),
    Completed       UMETA(DisplayName = "Completed"),
    Failed          UMETA(DisplayName = "Failed")
};

// Holds how many times a recipe is required (or delivered)
USTRUCT(BlueprintType)
struct SUSHIRESTAURANT_API FRecipeCount
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    URecipeData* Recipe = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Count = 0;

    FRecipeCount() {}
    FRecipeCount(URecipeData* InRecipe, const int32 InCount)
        : Recipe(InRecipe), Count(InCount) {}
    
    // Equality operator for use in array comparisons
    bool operator==(const FRecipeCount& Other) const
    {
        return Recipe == Other.Recipe && Count == Other.Count;
    }
};

// Represents an active customer order
USTRUCT(BlueprintType)
struct SUSHIRESTAURANT_API FActiveOrder
{
    GENERATED_BODY()

    // Recipes that need to be delivered
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FRecipeCount> RequiredRecipes;

    // Recipes already delivered
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FRecipeCount> DeliveredRecipes;

    // Time allowed to deliver the order (in seconds)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Order")
    float TotalTime = 60.f;

    // Timestamp when the order started (used for local progress calculation)
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float StartTime = 0.f;

    // Current progress state
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Order")
    EOrderStatus Status = EOrderStatus::None;

    FActiveOrder() {}

    FActiveOrder(const TArray<FRecipeCount>& InRequiredRecipes, const float InTotalTime, const float InStartTime)
        : RequiredRecipes(InRequiredRecipes), TotalTime(InTotalTime), StartTime(InStartTime), Status(EOrderStatus::Pending)
    {
    }

    // Clears the order to default state
    void Clear()
    {
        RequiredRecipes.Empty();
        DeliveredRecipes.Empty();
        TotalTime = 0.f;
        StartTime = 0.f;
        Status = EOrderStatus::None;
    }

    // Checks if the order is fully delivered
    bool IsComplete() const
    {
        for (const FRecipeCount& Req : RequiredRecipes)
        {
            if (GetDeliveredCountFor(Req.Recipe) < Req.Count)
            {
                return false;
            }
        }

        return true;
    }

    // Attempts to deliver a dish
    bool TryDeliverDish(const AActor* Dish)
    {
        if (!Dish || Status != EOrderStatus::Pending) return false;

        URecipeData* Recipe = nullptr;

        // Check if the actor has a recipe
        if (Dish->GetClass()->ImplementsInterface(URecipeSourceInterface::StaticClass()))
        {
            Recipe = IRecipeSourceInterface::Execute_GetRecipe(Dish);
        }

        if (!Recipe) return false;

        const int32 RequiredCount = GetRequiredCountFor(Recipe);
        if (RequiredCount == 0) return false;

        if (const int32 Delivered = GetDeliveredCountFor(Recipe); Delivered >= RequiredCount) return false;

        IncrementDelivered(Recipe);
        return true;
    }

    // How many of a recipe are required
    int32 GetRequiredCountFor(const URecipeData* Recipe) const
    {
        for (const FRecipeCount& Entry : RequiredRecipes)
        {
            if (Entry.Recipe == Recipe) return Entry.Count;
        }
        return 0;
    }

    // How many have already been delivered
    int32 GetDeliveredCountFor(const URecipeData* Recipe) const
    {
        for (const FRecipeCount& Entry : DeliveredRecipes)
        {
            if (Entry.Recipe == Recipe) return Entry.Count;
        }
        return 0;
    }

    // Increments delivered dishes count of a certain recipe
    void IncrementDelivered(URecipeData* Recipe)
    {
        for (FRecipeCount& Entry : DeliveredRecipes)
        {
            if (Entry.Recipe == Recipe)
            {
                Entry.Count++;
                return;
            }
        }

        DeliveredRecipes.Add(FRecipeCount(Recipe, 1));
    }

    // Calculate the time remaining (requires GetWorld()->GetTimeSeconds())
    float GetTimeRemaining(const float CurrentWorldTime) const
    {
        return FMath::Max(0.f, (StartTime + TotalTime) - CurrentWorldTime);
    }

    // Compares only the structure/identity of the order (not deliveries)
    bool IsNewComparedTo(const FActiveOrder& Other) const
    {
        if (Status != EOrderStatus::Pending || Other.Status != EOrderStatus::Pending)
        {
            return true;
        }

        if (!FMath::IsNearlyEqual(TotalTime, Other.TotalTime) ||
            !FMath::IsNearlyEqual(StartTime, Other.StartTime))
        {
            return true;
        }

        if (RequiredRecipes.Num() != Other.RequiredRecipes.Num())
        {
            return true;
        }

        for (int32 i = 0; i < RequiredRecipes.Num(); ++i)
        {
            if (RequiredRecipes[i] != Other.RequiredRecipes[i])
            {
                return true;
            }
        }

        return false;
    }

    // Compares two orders to determine if they are the same in structure and progress
    bool operator==(const FActiveOrder& Other) const
    {
        return Status == Other.Status &&
               FMath::IsNearlyEqual(TotalTime, Other.TotalTime) &&
               FMath::IsNearlyEqual(StartTime, Other.StartTime) &&
               RequiredRecipes == Other.RequiredRecipes &&
               DeliveredRecipes == Other.DeliveredRecipes;
    }

    bool operator!=(const FActiveOrder& Other) const
    {
        return !(*this == Other);
    }
};
