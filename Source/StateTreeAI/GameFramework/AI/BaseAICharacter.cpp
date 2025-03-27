// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseAICharacter.h"
#include "Components/StateTreeAIComponent.h"
#include "StateTreeAI/GameFramework/Components/PatrolComponent.h"

// Sets default values
ABaseAICharacter::ABaseAICharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	PatrolComponent = CreateDefaultSubobject<UPatrolComponent>("PatrolComponent");
}
