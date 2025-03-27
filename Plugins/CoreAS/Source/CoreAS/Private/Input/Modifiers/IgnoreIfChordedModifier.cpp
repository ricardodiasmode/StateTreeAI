// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/Modifiers/IgnoreIfChordedModifier.h"

#include "EnhancedPlayerInput.h"

FInputActionValue UIgnoreIfChordedModifier::ModifyRaw_Implementation(const UEnhancedPlayerInput* PlayerInput,
                                                                     FInputActionValue CurrentValue, float DeltaTime)
{
	if (PlayerInput && PlayerInput->IsPressed(ChordKey))
	{
		// If the chord key is held, cancel this input
		return FInputActionValue();
	}

	return CurrentValue;
}
