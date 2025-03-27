// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseAICharacter.generated.h"

class UPatrolComponent;

UCLASS()
class STATETREEAI_API ABaseAICharacter : public ACharacter
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly)
	UPatrolComponent* PatrolComponent = nullptr;
	
public:
	// Sets default values for this character's properties
	ABaseAICharacter();
	
	FORCEINLINE UPatrolComponent* GetPatrolComponent() const { return PatrolComponent; }

};
