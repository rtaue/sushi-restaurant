// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/Recipe/RecipeData.h"
#include "Interfaces/RecipeSourceInterface.h"
#include "SushiRestaurant/SushiRestaurant.h"
#include "ActiveOrder.generated.h"

UENUM(BlueprintType)
enum class EOrderStatus : uint8
{
    None = 0        UMETA(DisplayName = "None"),
    Pending         UMETA(DisplayName = "Pending"),
    Completed       UMETA(DisplayName = "Completed"),
    Failed          UMETA(DisplayName = "Failed")
};

// Represents a single recipe and how many times it is needed or delivered
USTRUCT(BlueprintType)
struct SUSHIRESTAURANT_API FRecipeCount
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    URecipeData* Recipe = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Count = 0;

    FRecipeCount() {}
    FRecipeCount(URecipeData* InRecipe, int32 InCount)
        : Recipe(InRecipe), Count(InCount) {}
};

// Holds the current order state: required dishes, delivered dishes, timer, and status
USTRUCT(BlueprintType)
struct SUSHIRESTAURANT_API FActiveOrder
{
    GENERATED_BODY()
    
    // Number of required dishes by recipe
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FRecipeCount> RequiredRecipes;

    // Number of delivered dishes so far
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FRecipeCount> DeliveredRecipes;

    // Remaining time before the order expires
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Order")
    float TimeRemaining = 0.f;

    // Current status of the order
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Order")
    EOrderStatus Status = EOrderStatus::None;

    FActiveOrder(){}

    // Constructor with multiple recipes and a timer
    FActiveOrder(const TArray<FRecipeCount>& InRequiredRecipes, const float InTimeRemaining)
        : RequiredRecipes(InRequiredRecipes), TimeRemaining(InTimeRemaining), Status(EOrderStatus::Pending) {}

    // Checks whether all required dishes have been delivered
    bool IsComplete() const
    {
        for (const FRecipeCount& Req : RequiredRecipes)
        {
            if (const int32 Delivered = GetDeliveredCountFor(Req.Recipe); Delivered < Req.Count)
            {
                UE_LOG(LogOrderSystem, Verbose, TEXT("Recipe %s not complete: %d/%d"),
                    *GetNameSafe(Req.Recipe), Delivered, Req.Count);

                GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Yellow,
                    FString::Printf(TEXT("Missing %s: %d/%d"),
                    *GetNameSafe(Req.Recipe), Delivered, Req.Count));
                
                return false;
            }
        }

        UE_LOG(LogOrderSystem, Log, TEXT("Order is complete"));
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Order Completed!"));
        
        return true;
    }

    // Attempts to deliver a dish to this order
    bool TryDeliverDish(const AActor* Dish)
    {
        if (!Dish || Status != EOrderStatus::Pending)
        {
            UE_LOG(LogOrderSystem, Warning, TEXT("TryDeliverDish failed: invalid dish or order not pending"));
            GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Red, TEXT("Can't deliver now."));
            
            return false;
        }
        
        URecipeData* Recipe = nullptr;
        if (Dish->GetClass()->ImplementsInterface(URecipeSourceInterface::StaticClass()))
        {
            Recipe = IRecipeSourceInterface::Execute_GetRecipe(Dish);
        }

        if (!Recipe)
        {
            UE_LOG(LogOrderSystem, Warning, TEXT("Dish does not implement IRecipeSourceInterface or returned null"));

            if (GEngine)
            {
                GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Red, TEXT("Invalid dish: no recipe found"));
            }
            
            return false;
        }

        const int32 RequiredCount = GetRequiredCountFor(Recipe);
        if (RequiredCount == 0)
        {
            UE_LOG(LogOrderSystem, Warning, TEXT("Recipe %s is not required in this order"), *GetNameSafe(Recipe));

            if (GEngine)
            {
                GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Red,
                    FString::Printf(TEXT("Recipe %s not required"), *GetNameSafe(Recipe)));
            }
            
            return false;
        }

        const int32 Delivered = GetDeliveredCountFor(Recipe);
        if (Delivered >= RequiredCount)
        {
            UE_LOG(LogOrderSystem, Warning, TEXT("All of recipe %s already delivered: %d/%d"),
                *GetNameSafe(Recipe), Delivered, RequiredCount);

            if (GEngine)
            {
                GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Red,
                    FString::Printf(TEXT("All of %s already delivered"), *GetNameSafe(Recipe)));
            }

            
            return false;
        }
        
        IncrementDelivered(Recipe);
        
        UE_LOG(LogOrderSystem, Log, TEXT("Delivered recipe %s (%d/%d)"),
            *GetNameSafe(Recipe), Delivered + 1, RequiredCount);

        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Cyan,
                FString::Printf(TEXT("Delivered %s (%d/%d)"), *GetNameSafe(Recipe), Delivered + 1, RequiredCount));
        }
        
        return true;
    }

private:

    // Gets how many of the given recipe are required in total
    int32 GetRequiredCountFor(const URecipeData* Recipe) const
    {
        for (const FRecipeCount& Entry : RequiredRecipes)
        {
            if (Entry.Recipe == Recipe)
            {
                return Entry.Count;
            }
        }
        return 0;
    }

    // Gets how many of the given recipe have been delivered
    int32 GetDeliveredCountFor(const URecipeData* Recipe) const
    {
        for (const FRecipeCount& Entry : DeliveredRecipes)
        {
            if (Entry.Recipe == Recipe)
            {
                return Entry.Count;
            }
        }
        return 0;
    }

    // Increments the delivered count for a recipe
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

        // If not found, add new
        DeliveredRecipes.Add(FRecipeCount(Recipe, 1));
    }

    // Display name helper
    FText GetDisplayName() const
    {
        if (RequiredRecipes.Num() == 1)
        {
            return RequiredRecipes[0].Recipe->DishName;
        }

        return FText::FromString("Combo Order");
    }
};
