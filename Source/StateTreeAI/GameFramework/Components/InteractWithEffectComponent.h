// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractWithEffectComponent.generated.h"


class AInteractableEffect;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STATETREEAI_API UInteractWithEffectComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	UPROPERTY()
	AInteractableEffect* CurrentEffect = nullptr;

public:
	// Sets default values for this component's properties
	UInteractWithEffectComponent();


public:
	void StartInteraction(AInteractableEffect* Effect);
	void FinishInteraction();
};
