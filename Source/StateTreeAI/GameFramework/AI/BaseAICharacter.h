// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffect.h"
#include "GameFramework/Character.h"
#include "StateTreeAI/Actors/InteractableEffect.h"
#include "StateTreeTasks/StateTreeTask_Feast.h"
#include "BaseAICharacter.generated.h"

class UBaseAttributeSet;
class UPatrolComponent;
class UNavComponentAS;

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

	UPROPERTY()
	class UInteractWithEffectComponent* InteractWithEffectComponent = nullptr;
	
protected:
	UPROPERTY(EditDefaultsOnly)
	UPatrolComponent* PatrolComponent = nullptr;

	UPROPERTY(EditDefaultsOnly)
	UNavComponentAS* NavComponent = nullptr;
	
	UPROPERTY(EditDefaultsOnly, Category="Config")
	UAnimMontage* FeastMontage = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="Config")
	float MaximumRecoveryDistance = 5000.f;

	UPROPERTY(EditDefaultsOnly, Category="AbilitySystem")
	TSubclassOf<UGameplayAbility> AttackAbility;
	UPROPERTY(EditDefaultsOnly, Category="AbilitySystem")
	TSubclassOf<UGameplayEffect> DefaultAttributeEffect;

private:
	void ActivateEffect(const TSubclassOf<UGameplayEffect>& Effect, AActor* Source);

	void InitAbilitySystem();
	
	UFUNCTION()
	void RecoveryFinish();

	virtual void HealthChange(const FOnAttributeChangeData& Data);

protected:
	virtual void BeginPlay() override;

	virtual void PossessedBy(AController* NewController) override;

public:
	// Sets default values for this character's properties
	ABaseAICharacter();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRecoveryFinishSignature);
	FOnRecoveryFinishSignature OnRecoveryFinishDelegate;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFinishInteractableEffectInteractionSignature);
	FOnFinishInteractableEffectInteractionSignature OnFinishInteractableEffectInteractionDelegate;
	
	void StartRecovery();

	void Attack();

	void StartFeast(AInteractableEffect* FeastActor);
	void OnFinishFeast();

	FORCEINLINE UPatrolComponent* GetPatrolComponent() const { return PatrolComponent; }

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const;

	virtual UBaseAttributeSet* GetAttributeSet() const;

	UFUNCTION(BlueprintCallable)
	void SetPatrolActor(class APatrolActor* PatrolActor);
	
};
