// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AITypes.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "Navigation/PathFollowingComponent.h"
#include "StateTreeTask_Feast.generated.h"

/**
 * 
 */
UCLASS()
class STATETREEAI_API UStateTreeTask_Feast : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

private:
	UPROPERTY()
	class ABaseAICharacter* AICharacter = nullptr;
	UPROPERTY()
	class AInteractableEffect* FeastActor = nullptr;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ExposeOnSpawn = "true"))
	class ABaseAIController* AIController = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ExposeOnSpawn = "true"))
	float SearchDistance = 1500.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ExposeOnSpawn = "true"))
	float FeastAcceptance = 120.f;

private:
	void FindFeastActorAround();
	
public:
	UFUNCTION()
	void OnFinishFeast();
	
	void MoveFinished(FAIRequestID FaiRequestID, const FPathFollowingResult& PathFollowingResult);
	
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
};
