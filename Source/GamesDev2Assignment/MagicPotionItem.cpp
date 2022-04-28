// Fill out your copyright notice in the Description page of Project Settings.


#include "MagicPotionItem.h"
#include "GamesDev2AssignmentCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AMagicPotionItem::AMagicPotionItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	itemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile Mesh"));
	itemMesh->SetupAttachment(RootComponent);
	itemMesh->SetNotifyRigidBodyCollision(true);
}

// Called when the game starts or when spawned
void AMagicPotionItem::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMagicPotionItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}