// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "AITypes.h"
#include "Navigation/PathFollowingComponent.h"
#include "StateTreeTask_ChaseActor.generated.h"

/**
 * 
 */
UCLASS()
class STATETREEAI_API UStateTreeTask_ChaseActor : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ExposeOnSpawn = "true"))
	class ABaseAIController* AIController = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ExposeOnSpawn = "true"))
	float Acceptance = 110.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ExposeOnSpawn = "true"))
	float ChaseSpeed = 600.f;

protected:
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) override;
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;

public:
	bool IsDistanceEnough();
	
	void MoveFinished(FAIRequestID FaiRequestID, const FPathFollowingResult& PathFollowingResult);
	
};
