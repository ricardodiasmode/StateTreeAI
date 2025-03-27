// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseAIController.h"

#include "Components/StateTreeAIComponent.h"


// Sets default values
ABaseAIController::ABaseAIController()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	StateTreeComponent = CreateDefaultSubobject<UStateTreeComponent>("StateTreeComponent");
}

// Called when the game starts or when spawned
void ABaseAIController::BeginPlay()
{
	Super::BeginPlay();

	StateTreeComponent->StartLogic();
	StateTreeComponent->RestartLogic();
}

