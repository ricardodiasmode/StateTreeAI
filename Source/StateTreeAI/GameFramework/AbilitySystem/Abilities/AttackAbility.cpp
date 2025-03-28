// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Misc/GeneralFunctionLibrary.h"
#include "StateTreeAI/GameFramework/AbilitySystem/Tags/CombatTags.h"

void UAttackAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                     const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                     const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		if (bDebug)
			GPrintDebug("Failed to activate ability. CommitAbility failed.");

		constexpr bool bReplicateEndAbility = true;
		constexpr bool bWasCancelled = true;

		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);
		return;
	}
	
	if (bDebug)
		GPrintDebug("CommitAbility success.");

	WaitAttackEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this,
		CombatTags::Combat_Attack,
		nullptr,
		false, 
		false);
	WaitAttackEvent->EventReceived.AddUniqueDynamic(this, &UAttackAbility::Attack);
	WaitAttackEvent->Activate();

	MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this,
		TEXT("AttackMontageTask"),
		AttackMontage,
		1,
		NAME_None,
		false);
	
	MontageTask->OnCancelled.AddUniqueDynamic(this, &UAttackAbility::OnMontageFinish);
	MontageTask->OnCompleted.AddUniqueDynamic(this, &UAttackAbility::OnMontageFinish);
	MontageTask->OnInterrupted.AddUniqueDynamic(this, &UAttackAbility::OnMontageFinish);
	MontageTask->OnBlendOut.AddUniqueDynamic(this, &UAttackAbility::OnMontageFinish);
	
	MontageTask->Activate();
}

void UAttackAbility::OnMontageFinish()
{
	constexpr bool bReplicateEndAbility = false;
	constexpr bool bWasCancelled = false;

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UAttackAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{	
	MontageTask->OnCancelled.RemoveDynamic(this, &UAttackAbility::OnMontageFinish);
	MontageTask->OnCompleted.RemoveDynamic(this, &UAttackAbility::OnMontageFinish);
	MontageTask->OnInterrupted.RemoveDynamic(this, &UAttackAbility::OnMontageFinish);
	MontageTask->OnBlendOut.RemoveDynamic(this, &UAttackAbility::OnMontageFinish);
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UAttackAbility::DamageHittedActors(FGameplayEventData& Payload, const TArray<FHitResult>& Hits)
{
	for (const FHitResult& CurrentHit : Hits)
	{
		if (!CurrentHit.GetActor())
			continue;
		
		FGameplayAbilityTargetingLocationInfo LocationInfo;
		LocationInfo.SourceActor = CurrentActorInfo->AvatarActor.Get();
		Payload.TargetData = LocationInfo.MakeTargetDataHandleFromActors({CurrentHit.GetActor()});
		ApplyGameplayEffectToTarget(CurrentSpecHandle,
									CurrentActorInfo,
									CurrentActivationInfo,
									Payload.TargetData,
									EffectOnTarget,
									1.f);
	}
}

void UAttackAbility::Attack(FGameplayEventData Payload)
{
	const FHitResult PayloadHitResult = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(Payload.TargetData, 0);

	const FVector HitForward = PayloadHitResult.ImpactNormal;
	const FVector HitLocation = PayloadHitResult.ImpactPoint;

	if (bDebug)
		GPrintDebug("Attacking...");

	if (!CurrentActorInfo->AvatarActor.Get())
	{
		GPrintError("Could not attack: !CurrentActorInfo->AvatarActor.Get()");
		return;
	}

	TArray<FHitResult> OutHits;
	const FVector StartLocation = HitLocation + HitForward * AttackRange;
	const FVector EndLocation = StartLocation + HitForward * AttackLength;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(CurrentActorInfo->AvatarActor.Get());
	
	UKismetSystemLibrary::CapsuleTraceMultiForObjects(GetWorld(),
													  StartLocation,
													  EndLocation,
													  AttackRadius,
													  AttackHeight,
													  ObjectTypes,
													  false,
													  ActorsToIgnore,
													  bDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
													  OutHits,
													  true);

	if (bDebug)
		GPrintDebugWithVar("On melee hit, found %d actors.", OutHits.Num());

	if (!OutHits.IsEmpty())
		DamageHittedActors(Payload, OutHits);
}
