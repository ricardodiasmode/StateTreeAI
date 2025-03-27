// Fill out your copyright notice in the Description page of Project Settings.


#include "NavigationFunctionLibrary.h"

#include "Components/NavComponentAS.h"

UNavComponentAS* UNavigationFunctionLibrary::MoveToActor(AActor* Who, AActor* Target,
		const float AcceptanceRadius,
		const float Deadline)
{
	if (!Who || !Target)
		return nullptr;

	if (!Who->GetComponentByClass<UNavComponentAS>())
		return nullptr;

	Who->GetComponentByClass<UNavComponentAS>()->WalkToActor(Target, AcceptanceRadius, Deadline);

	return Who->GetComponentByClass<UNavComponentAS>();
}

UNavComponentAS* UNavigationFunctionLibrary::MoveToLocation(AActor* Who, FVector Target,
		const float AcceptanceRadius,
		const float Deadline)
{
	if (!Who)
		return nullptr;

	if (!Who->GetComponentByClass<UNavComponentAS>())
		return nullptr;

	Who->GetComponentByClass<UNavComponentAS>()->WalkToLocation(Target, AcceptanceRadius, Deadline);

	return Who->GetComponentByClass<UNavComponentAS>();
}
