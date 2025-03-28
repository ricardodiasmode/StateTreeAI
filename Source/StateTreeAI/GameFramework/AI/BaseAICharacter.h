// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffect.h"
#include "GameFramework/Character.h"
#include "BaseAICharacter.generated.h"

class UBaseAttributeSet;
class UPatrolComponent;

UCLASS()
class STATETREEAI_API ABaseAICharacter : public ACharacter,
	public IAbilitySystemInterface
{
	GENERATED_BODY()

private:
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	
	UPROPERTY(Transient)
	TObjectPtr<UBaseAttributeSet> AttributeSet;

protected:
	UPROPERTY(EditDefaultsOnly)
	UPatrolComponent* PatrolComponent = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="AbilitySystem")
	TSubclassOf<UGameplayAbility> AttackAbility;
	UPROPERTY(EditDefaultsOnly, Category="AbilitySystem")
	TSubclassOf<UGameplayEffect> DefaultAttributeEffect;

private:
	void ActivateEffect(const TSubclassOf<UGameplayEffect>& Effect, AActor* Source);

	void InitAbilitySystem();

protected:
	virtual void BeginPlay() override;

public:
	// Sets default values for this character's properties
	ABaseAICharacter();

	void Attack();

	FORCEINLINE UPatrolComponent* GetPatrolComponent() const { return PatrolComponent; }

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const;

	virtual UBaseAttributeSet* GetAttributeSet() const;
};
