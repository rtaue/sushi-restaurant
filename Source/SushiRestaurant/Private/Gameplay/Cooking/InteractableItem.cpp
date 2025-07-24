// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Cooking/InteractableItem.h"


AInteractableItem::AInteractableItem()
{
	bReplicates = true;
	AActor::SetReplicateMovement(true);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);

	Mesh->SetIsReplicated(true);
	Mesh->SetSimulatePhysics(false);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AInteractableItem::Interact_Implementation(AActor* Interactor)
{
	// TODO: implement interact
}

void AInteractableItem::OnPickedUp_Implementation()
{
	// Disable physics and collision to prevent interference
	Mesh->SetSimulatePhysics(false);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	
	// Disable movement replication while being held
	SetReplicateMovement(false);
}

void AInteractableItem::OnDropped_Implementation()
{
	// For now keep disabled
	
	// Enable physics and collision for dropped items
	// Mesh->SetSimulatePhysics(true);
	// Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	// Mesh->SetCollisionResponseToAllChannels(ECR_Block);
	
	// Re-enable movement replication after drop
	// SetReplicateMovement(true);
}

