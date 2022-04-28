// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectileActor.generated.h"

class UProjectileMovementComponent;

UCLASS()
class GAMESDEV2ASSIGNMENT_API AProjectileActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectileActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		void SetDamage(float Damage);

private:
	UFUNCTION()
		void OnHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);
	
	UPROPERTY(EditAnywhere)
		float movementSpeed = 1000.0f;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* projectileMesh;

	UPROPERTY(EditAnywhere)
		float ProjectileDamage = 25.0f;

	UPROPERTY(EditAnywhere)
		UProjectileMovementComponent* projectileMovement;
};
