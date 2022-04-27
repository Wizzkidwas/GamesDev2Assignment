// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

class ASimpleAI_Character;

/**
 * 
 */
UCLASS()
class GAMESDEV2ASSIGNMENT_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	AActor* ChooseWaypoint();

	void RandomPatrol();

public:
	void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

private:
	APawn* PlayerPawn;

	ASimpleAI_Character* AICharacter;

	FTimerHandle TimerHandle;

	FTimerHandle ShootTimerHandle;

	bool waiting;

	bool chasing;

	UPROPERTY()
		TArray<AActor*> waypoints;
};
