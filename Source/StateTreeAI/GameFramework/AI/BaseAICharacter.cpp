// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseAICharacter.h"

#include "AbilitySystemComponent.h"
#include "Misc/GeneralFunctionLibrary.h"
#include "StateTreeAI/GameFramework/AbilitySystem/AttributeSets/BaseAttributeSet.h"
#include "StateTreeAI/GameFramework/Components/PatrolComponent.h"

// Sets default values
ABaseAICharacter::ABaseAICharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	PatrolComponent = CreateDefaultSubobject<UPatrolComponent>("PatrolComponent");
	
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>("AbilitySystemComponent");
	
	AttributeSet = CreateDefaultSubobject<UBaseAttributeSet>("AttributeSet");
}

void ABaseAICharacter::BeginPlay()
{
	Super::BeginPlay();

	InitAbilitySystem();
}

void ABaseAICharacter::InitAbilitySystem()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	ActivateEffect(DefaultAttributeEffect, this);

	AbilitySystemComponent->GiveAbility(AttackAbility);
}

void ABaseAICharacter::ActivateEffect(const TSubclassOf<UGameplayEffect>& Effect, AActor* Source)
{
	if (!IsValid(AbilitySystemComponent) || !IsValid(Effect))
		return;

	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(Source);
	FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(Effect, 1.f, EffectContext);

	if (!SpecHandle.IsValid())
		return;

	AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

void ABaseAICharacter::Attack()
{
	if (!IsValid(AbilitySystemComponent))
	{
		GPrintError("AI could not activate ability: !IsValid(AbilitySystemComponent");
		return;
	}

	if (!IsValid(AttackAbility))
	{
		GPrintError("NPC could not activate ability: !IsValid(AttackAbility)");
		return;
	}

	if (!AbilitySystemComponent->TryActivateAbilityByClass(AttackAbility))
	{
		GPrintErrorWithVar("Could not activate ability on AI: %s, probably ability was not given.", *UKismetSystemLibrary::GetDisplayName(this));
	}
}

UAbilitySystemComponent* ABaseAICharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UBaseAttributeSet* ABaseAICharacter::GetAttributeSet() const
{
	return AttributeSet;
}
