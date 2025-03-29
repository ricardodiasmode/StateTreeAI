// Fill out your copyright notice in the Description page of Project Settings.


#include "PatrolComponent.h"

#include "StateTreeAI/Actors/PatrolActor.h"


// Sets default values for this component's properties
UPatrolComponent::UPatrolComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UPatrolComponent::DefineCurrentPatrolPointToClosestPoint()
{
	if (!PatrolActor)
		return;
	
	if (PatrolActor->GetPatrolPoints().IsEmpty())
		return;
	
	CurrentPatrolPoint = 0;
	for (int i = 1; i < PatrolActor->GetPatrolPoints().Num(); i++)
	{
		const FVector& CurrentPoint = PatrolActor->GetPatrolPoints()[i];
		const float CheckDistance = FVector::Distance(CurrentPoint, GetOwner()->GetActorLocation());
		const float CurrentDistance = FVector::Distance(PatrolActor->GetPatrolPoints()[CurrentPatrolPoint], GetOwner()->GetActorLocation());
		if (CheckDistance < CurrentDistance)
			CurrentPatrolPoint = i;
	}
}

FVector UPatrolComponent::GetCurrentPatrolPointAndMoveToNext()
{
	if (!PatrolActor)
	{
		if (!GetOwner())
			return FVector::ZeroVector;
		return GetOwner()->GetActorLocation();
	}
	
	const FVector CurrentPoint = PatrolActor->GetPatrolPoints()[CurrentPatrolPoint];
	
	CurrentPatrolPoint++;
	if (CurrentPatrolPoint >= PatrolActor->GetPatrolPoints().Num())
		CurrentPatrolPoint = 0;

	return CurrentPoint;
}
