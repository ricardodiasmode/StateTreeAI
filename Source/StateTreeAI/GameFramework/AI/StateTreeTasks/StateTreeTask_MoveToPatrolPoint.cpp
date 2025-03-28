// Fill out your copyright notice in the Description page of Project Settings.


#include "StateTreeTask_MoveToPatrolPoint.h"

#include "AIController.h"
#include "Misc/GeneralFunctionLibrary.h"
#include "Navigation/PathFollowingComponent.h"
#include "StateTreeAI/GameFramework/AI/BaseAICharacter.h"
#include "StateTreeAI/GameFramework/Components/PatrolComponent.h"

EStateTreeRunStatus UStateTreeTask_MoveToPatrolPoint::EnterState(FStateTreeExecutionContext& Context,
                                                                 const FStateTreeTransitionResult& Transition)
{
	Super::EnterState(Context, Transition);
	
	if (!AIController)
	{
		GPrintError("!AIController on UStateTreeTask_MoveToPatrolPoint::EnterState. Will not proceed.");
		return EStateTreeRunStatus::Failed;
	}
	
	ABaseAICharacter* AICharacter = Cast<ABaseAICharacter>(AIController->GetCharacter());
	if (!AICharacter)
	{
		GPrintError("!AICharacter on UStateTreeTask_MoveToPatrolPoint::EnterState. Will not proceed.");
		return EStateTreeRunStatus::Failed;
	}
	
	AIController->GetPathFollowingComponent()->OnRequestFinished.AddUObject(this, &UStateTreeTask_MoveToPatrolPoint::MoveFinished);

	UPatrolComponent* CharacterPatrolComponent = AICharacter->GetPatrolComponent();
	const FVector PatrolPoint = CharacterPatrolComponent->GetCurrentPatrolPointAndMoveToNext();
	AIController->MoveToLocation(PatrolPoint,
		Acceptance,
		false);
	
	return EStateTreeRunStatus::Running;
}

void UStateTreeTask_MoveToPatrolPoint::MoveFinished(FAIRequestID FaiRequestID,
	const FPathFollowingResult& PathFollowingResult)
{
	GPrintDebugWithVar("path following result: %s", *UEnum::GetValueAsString(PathFollowingResult.Code));
	FinishTask(PathFollowingResult.IsSuccess());
}
