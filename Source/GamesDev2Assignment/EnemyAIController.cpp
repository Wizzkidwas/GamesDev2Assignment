// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/TargetPoint.h"
#include "Engine/EngineTypes.h"
#include "SimpleAI_Character.h"
#include "ProjectileActor.h"

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();
	PrimaryActorTick.bCanEverTick = true;
	PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	AICharacter = Cast<ASimpleAI_Character>(GetPawn());
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATargetPoint::StaticClass(), waypoints);
	RandomPatrol();
	waiting = false;
}

void AEnemyAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// if (PlayerPawn) SetFocus(PlayerPawn);
	if (LineOfSightTo(PlayerPawn, FVector(0, 0, 0)) && !waiting && !dead)
	{
		FVector AIForwardVector = AICharacter->GetActorForwardVector(); // Already nomalised
		FVector PlayerPositionVector = PlayerPawn->GetActorLocation();
		FVector AIPositionVector = AICharacter->GetActorLocation();
		FVector AIToPlayerVector = PlayerPositionVector - AIPositionVector;
		AIToPlayerVector.Normalize(); // This vector must be explicitly normalised
		float DirectionDotProduct = FVector::DotProduct(AIToPlayerVector, AIForwardVector);

		if (DirectionDotProduct > 0.4f)
		{
			MoveToActor(PlayerPawn, 10.0f);
			UE_LOG(LogTemp, Warning, TEXT("PLAYER SPOTTED"))
			GetWorld()->GetTimerManager().SetTimer(ShootTimerHandle, [&]()
			{
				if (!dead)
				{
					AICharacter->Shoot();
					UE_LOG(LogTemp, Warning, TEXT("SHOOT"))
				}
			}, 1, true, 1);
		}
	}
}

AActor* AEnemyAIController::ChooseWaypoint()
{
	int index = FMath::RandRange(0, waypoints.Num() - 1);
	return waypoints[index];
}

void AEnemyAIController::RandomPatrol()
{
	MoveToActor(ChooseWaypoint(), 10.0f);
	UE_LOG(LogTemp, Warning, TEXT("Begin next movement"))
	waiting = false;
}

void AEnemyAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);
	waiting = true;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AEnemyAIController::RandomPatrol, 6.0f, false, 6.0f);
}

void AEnemyAIController::StopTimers()
{
	dead = true;
	GetWorldTimerManager().ClearTimer(ShootTimerHandle);
	GetWorldTimerManager().ClearTimer(TimerHandle);
}

