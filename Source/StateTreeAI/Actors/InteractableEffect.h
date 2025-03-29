// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableEffect.generated.h"

enum class EInteractableType : uint8;
class UGameplayEffect;


UCLASS()
class STATETREEAI_API AInteractableEffect : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, Category="Config")
	TSubclassOf<UGameplayEffect> Effect;
	
	UPROPERTY(EditAnywhere, Category="Config")
	EInteractableType Type;

public:
	// Sets default values for this actor's properties
	AInteractableEffect();

	FORCEINLINE EInteractableType GetType() const { return Type; }
	FORCEINLINE TSubclassOf<UGameplayEffect> GetEffect() const { return Effect; }

};
