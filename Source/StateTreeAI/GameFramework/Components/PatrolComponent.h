// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PatrolComponent.generated.h"


class APatrolActor;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STATETREEAI_API UPatrolComponent : public UActorComponent
{
	GENERATED_BODY()

	int CurrentPatrolPoint = -1;

public:
	UPROPERTY(EditAnywhere, Category="Config|Patrol")
	APatrolActor* PatrolActor = nullptr;

public:
	// Sets default values for this component's properties
	UPatrolComponent();

	void DefineCurrentPatrolPointToClosestPoint();
	
	FVector GetCurrentPatrolPointAndMoveToNext();

};
