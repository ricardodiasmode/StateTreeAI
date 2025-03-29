// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractWithEffectComponent.h"

#include "AbilitySystemComponent.h"
#include "StateTreeAI/Actors/InteractableEffect.h"


// Sets default values for this component's properties
UInteractWithEffectComponent::UInteractWithEffectComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UInteractWithEffectComponent::StartInteraction(AInteractableEffect* Effect)
{
	CurrentEffect = Effect;
}

void UInteractWithEffectComponent::FinishInteraction()
{
	if (UAbilitySystemComponent* ASC = GetOwner()->GetComponentByClass<UAbilitySystemComponent>())
	{
		FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
		EffectContext.AddSourceObject(GetOwner());
		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(CurrentEffect->GetEffect(), 1.f, EffectContext);

		if (SpecHandle.IsValid())
			ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
		
	CurrentEffect->Destroy();
}

