// Fill out your copyright notice in the Description page of Project Settings.


#include "StateTreeTask_StartPatrol.h"

#include "AIController.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Misc/GeneralFunctionLibrary.h"
#include "StateTreeAI/GameFramework/AI/BaseAICharacter.h"
#include "StateTreeAI/GameFramework/Components/PatrolComponent.h"

EStateTreeRunStatus UStateTreeTask_StartPatrol::EnterState(FStateTreeExecutionContext& Context,
                                                          const FStateTreeTransitionResult& Transition)
{
	Super::EnterState(Context, Transition);
	
	if (!AIController) // Not error because can happen if AI just spawns
		return EStateTreeRunStatus::Failed;
	
	ABaseAICharacter* AICharacter = Cast<ABaseAICharacter>(AIController->GetCharacter());
	if (!AICharacter) // Not error because can happen if AI just spawns
		return EStateTreeRunStatus::Failed;

	AICharacter->GetCharacterMovement()->MaxWalkSpeed = PatrolSpeed;

	UPatrolComponent* CharacterPatrolComponent = AICharacter->GetPatrolComponent();
	CharacterPatrolComponent->DefineCurrentPatrolPointToClosestPoint();

	// on next task, call MoveTo on next patrol point, then after move success increment the patrol point
	FinishTask(true);
	return EStateTreeRunStatus::Succeeded;
}
