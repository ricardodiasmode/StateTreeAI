// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "StateTreeTask_Attack.generated.h"

/**
 * 
 */
UCLASS()
class STATETREEAI_API UStateTreeTask_Attack : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ExposeOnSpawn = "true"))
	class ABaseAIController* AIController = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ExposeOnSpawn = "true"))
	float DistanceToHitAgain = 150.f;

	void WaitAttackEnd(const FAbilityEndedData& AbilityEndedData);
	
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
};
