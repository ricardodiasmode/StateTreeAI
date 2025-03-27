// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NavigationFunctionLibrary.generated.h"

class UNavComponentAS;

UCLASS()
class NAVIGATIONAS_API UNavigationFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	static UNavComponentAS* MoveToActor(AActor* Who, AActor* Target,
		const float AcceptanceRadius = 15.f,
		const float Deadline = 15.f);
	UFUNCTION(BlueprintCallable)
	static UNavComponentAS* MoveToLocation(AActor* Who, FVector Target,
		const float AcceptanceRadius = 15.f,
		const float Deadline = 15.f );
};
