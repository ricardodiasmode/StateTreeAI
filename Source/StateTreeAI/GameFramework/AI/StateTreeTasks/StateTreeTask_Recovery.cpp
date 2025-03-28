// Fill out your copyright notice in the Description page of Project Settings.


#include "StateTreeTask_Recovery.h"

#include "Misc/GeneralFunctionLibrary.h"
#include "StateTreeAI/GameFramework/AI/BaseAICharacter.h"
#include "StateTreeAI/GameFramework/AI/BaseAIController.h"

EStateTreeRunStatus UStateTreeTask_Recovery::EnterState(FStateTreeExecutionContext& Context,
                                                        const FStateTreeTransitionResult& Transition)
{
	Super::EnterState(Context, Transition);

	if (!AIController)
	{
		GPrintError("!AIController on UStateTreeTask_Recovery::EnterState. Will not proceed.");
		return EStateTreeRunStatus::Failed;
	}
	
	ABaseAICharacter* AICharacter = Cast<ABaseAICharacter>(AIController->GetCharacter());
	if (!AICharacter)
	{
		GPrintError("!AICharacter on UStateTreeTask_Recovery::EnterState. Will not proceed.");
		return EStateTreeRunStatus::Failed;
	}

	AICharacter->OnRecoveryFinishDelegate.AddUniqueDynamic(this, &UStateTreeTask_Recovery::FinishRecovery);
	AICharacter->StartRecovery();
	
	return EStateTreeRunStatus::Running;
}

void UStateTreeTask_Recovery::FinishRecovery()
{
	GPrintDebug("finish recovery");
	FinishTask(true); // we will not handle failure because this is the last resort
}
