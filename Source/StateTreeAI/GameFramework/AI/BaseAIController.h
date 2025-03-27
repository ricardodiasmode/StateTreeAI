// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "BaseAIController.generated.h"

UCLASS()
class STATETREEAI_API ABaseAIController : public AAIController
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category="Config")
	class UStateTreeComponent* StateTreeComponent = nullptr;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	AActor* Target = nullptr;

public:
	UFUNCTION()
	void PerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
	
	// Sets default values for this actor's properties
	ABaseAIController();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
