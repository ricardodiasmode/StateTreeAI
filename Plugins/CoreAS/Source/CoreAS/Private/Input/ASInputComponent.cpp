// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/ASInputComponent.h"


// Sets default values for this component's properties
UASInputComponent::UASInputComponent(const FObjectInitializer& ObjectInitializer)
{
}

void UASInputComponent::AddInputMappings(const UASInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
	check(InputConfig);
	check(InputSubsystem);

	// Here you can handle any custom logic to add something from your input config if required
}

void UASInputComponent::RemoveInputMappings(const UASInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
	check(InputConfig);
	check(InputSubsystem);

	// Here you can handle any custom logic to remove input mappings that you may have added above
}

void UASInputComponent::RemoveBinds(TArray<uint32>& BindHandles)
{
	for (uint32 Handle : BindHandles)
	{
		RemoveBindingByHandle(Handle);
	}
	BindHandles.Reset();
}

