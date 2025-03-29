// Fill out your copyright notice in the Description page of Project Settings.


#include "StateTreeTask_Feast.h"

#include "Misc/GeneralFunctionLibrary.h"
#include "Navigation/PathFollowingComponent.h"
#include "StateTreeAI/Actors/InteractableEffect.h"
#include "StateTreeAI/GameFramework/AI/BaseAICharacter.h"
#include "StateTreeAI/GameFramework/AI/BaseAIController.h"
#include "StateTreeAI/GameFramework/Enums/InteractableType.h"

class ABaseAICharacter;

EStateTreeRunStatus UStateTreeTask_Feast::EnterState(FStateTreeExecutionContext& Context,
                                                     const FStateTreeTransitionResult& Transition)
{
	Super::EnterState(Context, Transition);
	
	if (!AIController)
	{
		GPrintError("!AIController on UStateTreeTask_MoveToPatrolPoint::EnterState. Will not proceed.");
		return EStateTreeRunStatus::Failed;
	}
	
	AICharacter = Cast<ABaseAICharacter>(AIController->GetCharacter());
	if (!AICharacter)
	{
		GPrintError("!AICharacter on UStateTreeTask_MoveToPatrolPoint::EnterState. Will not proceed.");
		return EStateTreeRunStatus::Failed;
	}

	FindFeastActorAround();

	if (!FeastActor)
	{
		GPrintDebug("failed feast because could not find feast actor around");
		return EStateTreeRunStatus::Failed;
	}
	GPrintDebug("feast found feast actor");
	
	AIController->GetPathFollowingComponent()->OnRequestFinished.AddUObject(this, &UStateTreeTask_Feast::MoveFinished);

	AIController->MoveToLocation(FeastActor->GetActorLocation(),
		FeastAcceptance,
		false);
	
	return EStateTreeRunStatus::Running;
}

void UStateTreeTask_Feast::FindFeastActorAround()
{
	if (!AICharacter)
		return;

	TArray<FHitResult> OutHits;
	UKismetSystemLibrary::SphereTraceMulti(GetWorld(),
		AICharacter->GetActorLocation(),
		AICharacter->GetActorLocation() + 1.f,
		SearchDistance,
		UEngineTypes::ConvertToTraceType(ECC_Visibility), // this could be a specific channel to feast interactable
		false,
		{AICharacter},
		EDrawDebugTrace::None,
		OutHits,
		true);

	for (const FHitResult& CurrentHit : OutHits)
	{
		if (AInteractableEffect* FoundInteractable = Cast<AInteractableEffect>(CurrentHit.GetActor()))
		{
			if (FoundInteractable->GetType() == EInteractableType::FEAST)
			{
				FeastActor = FoundInteractable;
				return;
			}
		}
	}
}

void UStateTreeTask_Feast::OnFinishFeast()
{
	FinishTask(true);
}

void UStateTreeTask_Feast::MoveFinished(FAIRequestID FaiRequestID, const FPathFollowingResult& PathFollowingResult)
{
	if (!AICharacter || PathFollowingResult.IsFailure() || !FeastActor)
	{
		FinishTask(false);
		return;
	}
	
	AICharacter->OnFinishInteractableEffectInteractionDelegate.AddUniqueDynamic(this, &UStateTreeTask_Feast::OnFinishFeast);
	AICharacter->StartFeast(FeastActor);
}
