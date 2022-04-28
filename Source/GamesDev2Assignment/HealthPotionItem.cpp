// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthPotionItem.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GamesDev2AssignmentCharacter.h"

// Sets default values
AHealthPotionItem::AHealthPotionItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	itemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile Mesh"));
	itemMesh->SetupAttachment(RootComponent);
	itemMesh->SetNotifyRigidBodyCollision(true);
}

// Called when the game starts or when spawned
void AHealthPotionItem::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AHealthPotionItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
