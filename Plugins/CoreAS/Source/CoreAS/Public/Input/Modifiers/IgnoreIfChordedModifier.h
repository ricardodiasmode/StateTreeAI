// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputModifiers.h"
#include "IgnoreIfChordedModifier.generated.h"

/**
 * 
 */
UCLASS()
class COREAS_API UIgnoreIfChordedModifier : public UInputModifier
{
	GENERATED_BODY()

public:
	// Key that needs to be pressed to ignore this action
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Modifier")
	FKey ChordKey;

	virtual FInputActionValue ModifyRaw_Implementation(const UEnhancedPlayerInput* PlayerInput, FInputActionValue CurrentValue, float DeltaTime) override;
};
