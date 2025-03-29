// Fill out your copyright notice in the Description page of Project Settings.


#include "StateTreeTask_Attack.h"

#include "AbilitySystemComponent.h"
#include "Misc/GeneralFunctionLibrary.h"
#include "StateTreeAI/GameFramework/AI/BaseAICharacter.h"
#include "StateTreeAI/GameFramework/AI/BaseAIController.h"

class ABaseAICharacter;

EStateTreeRunStatus UStateTreeTask_Attack::EnterState(FStateTreeExecutionContext& Context,
                                                      const FStateTreeTransitionResult& Transition)
{
	Super::EnterState(Context, Transition);
	
	if (!AIController)
	{
		GPrintError("!AIController on UStateTreeTask_Attack::EnterState. Will not proceed.");
		return EStateTreeRunStatus::Failed;
	}
	
	ABaseAICharacter* AICharacter = Cast<ABaseAICharacter>(AIController->GetCharacter());
	if (!AICharacter)
	{
		GPrintError("!AICharacter on UStateTreeTask_Attack::EnterState. Will not proceed.");
		return EStateTreeRunStatus::Failed;
	}

	AICharacter->GetAbilitySystemComponent()->OnAbilityEnded.AddUObject(this, &UStateTreeTask_Attack::WaitAttackEnd);
	AICharacter->Attack();

	return EStateTreeRunStatus::Running;
}

void UStateTreeTask_Attack::WaitAttackEnd(const FAbilityEndedData& AbilityEndedData)
{
	if (!AIController || !AIController->Target)
	{
		GPrintError("!AIController || !AIController->Target on UStateTreeTask_Attack::WaitAttackEnd. Will not proceed.");
		FinishTask(false);
		return;
	}
	
	ABaseAICharacter* AICharacter = Cast<ABaseAICharacter>(AIController->GetCharacter());
	if (!AICharacter)
	{ // Not an error because this can happen
		FinishTask(false);
		return;
	}

	AICharacter->GetAbilitySystemComponent()->OnAbilityEnded.RemoveAll(this);
	const bool IsDistanceEnough = AICharacter->GetDistanceTo(AIController->Target) < DistanceToHitAgain;
	FinishTask(!AbilityEndedData.bWasCancelled && IsDistanceEnough);
}
