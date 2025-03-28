// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "AttackAbility.generated.h"

class UAbilityTask_WaitGameplayEvent;
class UAbilityTask_PlayMontageAndWait;
/**
 * 
 */
UCLASS()
class STATETREEAI_API UAttackAbility : public UGameplayAbility
{
	GENERATED_BODY()
private:
	UPROPERTY()
	UAbilityTask_PlayMontageAndWait* MontageTask = nullptr;
	UPROPERTY()
	UAbilityTask_WaitGameplayEvent* WaitAttackEvent = nullptr;

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> EffectOnTarget;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> AttackMontage;
	
	UPROPERTY(EditDefaultsOnly)
	float AttackRange = 50.f;
	
	UPROPERTY(EditDefaultsOnly)
	float AttackLength = 150.f;
	
	UPROPERTY(EditDefaultsOnly)
	float AttackRadius = 120.f;
	
	UPROPERTY(EditDefaultsOnly)
	float AttackHeight = 90.f;
	
	UPROPERTY(EditDefaultsOnly)
	uint8 bDebug : 1 = false;

private:
	void DamageHittedActors(FGameplayEventData& Payload, const TArray<FHitResult>& Hits);
	
public:
	UFUNCTION()
	void Attack(FGameplayEventData Payload);
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION()
	void OnMontageFinish();
};
