// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AITypes.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "Navigation/PathFollowingComponent.h"
#include "StateTreeTask_MoveToPatrolPoint.generated.h"

/**
 * 
 */
UCLASS()
class STATETREEAI_API UStateTreeTask_MoveToPatrolPoint : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ExposeOnSpawn = "true"))
	class AAIController* AIController = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ExposeOnSpawn = "true"))
	float Acceptance = 90.f;

public:
	void MoveFinished(FAIRequestID FaiRequestID, const FPathFollowingResult& PathFollowingResult);
	
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;

};
