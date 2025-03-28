// Fill out your copyright notice in the Description page of Project Settings.


#include "StateTreeTask_ChaseActor.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Misc/GeneralFunctionLibrary.h"
#include "Navigation/PathFollowingComponent.h"
#include "StateTreeAI/GameFramework/AI/BaseAICharacter.h"
#include "StateTreeAI/GameFramework/AI/BaseAIController.h"

EStateTreeRunStatus UStateTreeTask_ChaseActor::EnterState(FStateTreeExecutionContext& Context,
                                                          const FStateTreeTransitionResult& Transition)
{
	Super::EnterState(Context, Transition);
	
	if (!AIController)
	{
		GPrintError("!AIController on UStateTreeTask_ChaseActor::EnterState. Will not proceed.");
		return EStateTreeRunStatus::Failed;
	}
	ABaseAICharacter* AICharacter = Cast<ABaseAICharacter>(AIController->GetCharacter());
	if (!AICharacter)
	{
		GPrintError("!AICharacter on UStateTreeStartPatrolTask::EnterState. Will not proceed.");
		return EStateTreeRunStatus::Failed;
	}

	AICharacter->GetCharacterMovement()->MaxWalkSpeed = ChaseSpeed;
	
	AIController->GetPathFollowingComponent()->OnRequestFinished.AddUObject(this, &UStateTreeTask_ChaseActor::MoveFinished);

	AIController->MoveToActor(AIController->Target,
		Acceptance*0.75,
		false);
	
	return EStateTreeRunStatus::Running;
}

void UStateTreeTask_ChaseActor::MoveFinished(FAIRequestID FaiRequestID,
	const FPathFollowingResult& PathFollowingResult)
{
	if (!AIController || !AIController->GetCharacter() || !AIController->Target)
	{
		GPrintError("Actor nullptr on UStateTreeTask_ChaseActor::MoveFinished. Will not proceed.");
		FinishTask(false);
		return;
	}

	if (AIController->GetCharacter())
	{
		if (FVector::Distance(AIController->GetCharacter()->GetActorLocation(), AIController->Target->GetActorLocation()) <= Acceptance)
		{
			FinishTask(true);
			return;
		}
	}
	
	FinishTask(false);
}

