﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseAIController.h"

#include "BaseAICharacter.h"
#include "Components/StateTreeAIComponent.h"
#include "Misc/GeneralFunctionLibrary.h"
#include "Perception/AIPerceptionComponent.h"
#include "StateTreeAI/GameFramework/GameplayCharacter.h"
#include "StateTreeAI/GameFramework/AbilitySystem/AttributeSets/BaseAttributeSet.h"

// Sets default values
ABaseAIController::ABaseAIController()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	StateTreeComponent = CreateDefaultSubobject<UStateTreeComponent>("StateTreeComponent");

	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));
	PerceptionComponent->OnTargetPerceptionUpdated.AddUniqueDynamic(this, &ABaseAIController::PerceptionUpdated);
}

// Called when the game starts or when spawned
void ABaseAIController::BeginPlay()
{
	Super::BeginPlay();

	StateTreeComponent->StartLogic();
	StateTreeComponent->RestartLogic();
}

void ABaseAIController::PerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (!Actor->IsA(AGameplayCharacter::StaticClass()))
		return;
	
	if (!Target && Stimulus.WasSuccessfullySensed())
	{
		Target = Actor;
	} else
	{
		Target = nullptr;
	}
}

void ABaseAIController::HealthChange(const float NewValue)
{
	Health = NewValue;
}
