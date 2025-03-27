// Fill out your copyright notice in the Description page of Project Settings.


#include "GeneralFunctionLibrary.h"

#include "Abilities/GameplayAbility.h"

TSubclassOf<UGameplayEffect> UGeneralFunctionLibrary::GetCostGameplayEffectClass(
	const TSubclassOf<UGameplayAbility>& AbilityClass)
{
	if (!AbilityClass)
	{
		return nullptr;
	}

	const UGameplayAbility* DefaultAbility = AbilityClass->GetDefaultObject<UGameplayAbility>();
	if (!DefaultAbility)
	{
		if(GEngine){GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("Failed to get default obj on UGeneralFunctionLibrary::GetCostGameplayEffectClass."));}
		return nullptr;
	}

	if (DefaultAbility->GetCostGameplayEffect())
		return DefaultAbility->GetCostGameplayEffect()->GetClass();

	return nullptr;
}

TSubclassOf<UGameplayEffect> UGeneralFunctionLibrary::GetCooldownGameplayEffectClass(
	const TSubclassOf<UGameplayAbility>& AbilityClass)
{
	if (!AbilityClass)
	{
		return nullptr;
	}

	const UGameplayAbility* DefaultAbility = AbilityClass->GetDefaultObject<UGameplayAbility>();
	if (!DefaultAbility)
	{
		if(GEngine){GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("Failed to get default obj on UGeneralFunctionLibrary::GetCooldownGameplayEffectClass."));}
		return nullptr;
	}

	if (DefaultAbility->GetCooldownGameplayEffect())
		return DefaultAbility->GetCooldownGameplayEffect()->GetClass();

	return nullptr;
}

UStaticMeshComponent* UGeneralFunctionLibrary::GetClosestComponentToLocation(TArray<UStaticMeshComponent*> ComponentArray, FVector Location)
{
	if (ComponentArray.IsEmpty())
		return nullptr;
	
	UStaticMeshComponent* ClosestComponent = ComponentArray[0];
	for (UStaticMeshComponent* CurrentComponent : ComponentArray)
	{
		if (FVector::Dist(CurrentComponent->GetComponentLocation(), Location) <
			FVector::Dist(ClosestComponent->GetComponentLocation(), Location))
				ClosestComponent = CurrentComponent;
	}

	return ClosestComponent;
}

int UGeneralFunctionLibrary::Fibonacci(const int n)
{
	if (n <= 1) 
		return 1;

	int prev1 = 1, prev2 = 1; // Base cases: Fibonacci(0) and Fibonacci(1)
	int current = 0;

	for (int i = 2; i <= n; i++) {
		current = prev1 + prev2; // Calculate Fibonacci(i)
		prev2 = prev1;           // Update prev2 to the last value
		prev1 = current;         // Update prev1 to the current value
	}

	return current; // Return Fibonacci(n)
}

bool UGeneralFunctionLibrary::HasAnyObjectBetween(AActor* First, AActor* Second)
{
	FHitResult OutHit;
	UKismetSystemLibrary::LineTraceSingle(First->GetWorld(),
		First->GetActorLocation(),
		Second->GetActorLocation(),
		UEngineTypes::ConvertToTraceType(ECC_Visibility),
		false,
		{First, Second},
		EDrawDebugTrace::None,
		OutHit,
		true);
	return OutHit.bBlockingHit;
}

float UGeneralFunctionLibrary::DistanceBetween(AActor* First, AActor* Second)
{
	return FVector::Dist(First->GetActorLocation(), Second->GetActorLocation());
}
