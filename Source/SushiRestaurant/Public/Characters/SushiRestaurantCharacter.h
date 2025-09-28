// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Gameplay/Equip/EquippedVisualData.h"
#include "Logging/LogMacros.h"
#include "SushiRestaurantCharacter.generated.h"

enum class EEquippedVisual : uint8;
class USpringArmComponent;
class UCameraComponent;
class UInputAction;
struct FInputActionValue;

/**
 * A player-controllable character for the sushi restaurant game.
 * Handles movement, item holding, and interaction visuals in a multiplayer environment.
 */
UCLASS(abstract)
class ASushiRestaurantCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
protected:

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* LookAction;

	/** Mouse Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* MouseLookAction;

public:

	/** Constructor */
	ASushiRestaurantCharacter();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:

	/** Initialize input action bindings */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

public:

	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	/** Handles look inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoLook(float Yaw, float Pitch);

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpStart();

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpEnd();

public:

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

#pragma region Inventory and Interaction

public:
    /** Pick up a specified item. Must be called on the server. */
    void PickupItem(AActor* Item);

    /** Drop the currently held item. Must be called on the server. */
    void DropItem();

    /** Checks if the character is currently holding an item. */
    UFUNCTION(BlueprintPure, Category = "Inventory")
    bool IsHoldingItem() const { return HeldItem != nullptr; }

	/**
	 * Returns the actor currently being held by the character.
	 * @return A pointer to the held actor, or nullptr if nothing is being held.
	 */
	UFUNCTION(BlueprintPure, Category = "Inventory")
	AActor* GetHeldItem() const { return HeldItem; }

    /** Lock the character to an interaction station. Must be called on the server. */
    void LockToStation(AActor* Station);

    /** Unlock the character from an interaction station. Must be called on the server. */
    void UnlockFromStation();

    /** Checks if the character is currently locked to a station. */
    UFUNCTION(BlueprintPure, Category = "Interaction")
    bool IsLocked() const { return LockedStation != nullptr; }

    /**
     * Requests to play an interaction, including an animation and an optional equipped visual.
     * This is the main entry point for interactions, which will handle network replication.
     * @param Montage The animation montage to play.
     * @param Equip The visual item to show in the character's hands.
     */
    UFUNCTION(BlueprintCallable, Category = "Interaction")
    void RequestInteraction(UAnimMontage* Montage, const EEquippedVisual& Equip = EEquippedVisual::None);

    /**
     * Requests to stop the current interaction, stopping the animation and hiding any equipped visual.
     */
    UFUNCTION(BlueprintCallable, Category = "Interaction")
    void RequestStopInteraction();

protected:
    // The currently held item (e.g., a plate, an ingredient). Replicated to all clients.
    UPROPERTY(ReplicatedUsing = OnRep_HeldItem, BlueprintReadOnly, Category = "Inventory")
    TObjectPtr<AActor> HeldItem;

    // The station the character is currently interacting with. Not replicated as it's server-side logic.
    UPROPERTY()
    TObjectPtr<AActor> LockedStation;

private:
    /** Single RPC to handle starting an interaction on the server. */
    UFUNCTION(Server, Reliable)
    void Server_SetInteractionState(UAnimMontage* Montage, EEquippedVisual Equip);

    /** Single RPC to handle stopping an interaction on the server. */
    UFUNCTION(Server, Reliable)
    void Server_ClearInteractionState();
    
    /** Plays the animation montage on all clients. */
    UFUNCTION(NetMulticast, Reliable)
    void Multicast_PlayMontage(UAnimMontage* Montage);

    /** Stops the current animation montage on all clients. */
    UFUNCTION(NetMulticast, Reliable)
    void Multicast_StopMontage();
    
    /** The visual item currently equipped (e.g., knife, pan). Replicated to all clients. */
    UPROPERTY(ReplicatedUsing = OnRep_EquippedVisual)
    EEquippedVisual EquippedVisual;

    /** Called on clients when the EquippedVisual variable changes. */
    UFUNCTION()
    void OnRep_EquippedVisual();
    
    /** Called on clients when the HeldItem variable changes. */
    UFUNCTION()
    void OnRep_HeldItem();

    /**
     * Updates the static mesh component based on the current value of EquippedVisual.
     * Called by OnRep_EquippedVisual on all machines.
     */
    void UpdateEquippedVisual();

    /** The component used to display the equipped visual (e.g., knife mesh). */
    UPROPERTY(VisibleAnywhere, Category = "Visual")
    TObjectPtr<UStaticMeshComponent> EquipVisualMesh;
    
    /** DataTable containing the mapping from EEquippedVisual enum to a static mesh. */
    UPROPERTY(EditDefaultsOnly, Category = "Visual")
    TObjectPtr<UDataTable> EquippedVisualTable;
    
    /** Scene component used as an anchor point for held items. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USceneComponent> ItemAttachPoint;

#pragma endregion
};

