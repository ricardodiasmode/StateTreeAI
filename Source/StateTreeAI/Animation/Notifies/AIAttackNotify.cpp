// Fill out your copyright notice in the Description page of Project Settings.


#include "AIAttackNotify.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Misc/GeneralFunctionLibrary.h"
#include "StateTreeAI/GameFramework/AbilitySystem/Tags/CombatTags.h"

void UAIAttackNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                             const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp)
		return;
	if (!MeshComp->GetOwner())
		return;
	if (!MeshComp->GetOwner()->GetComponentByClass<UAbilitySystemComponent>())
		return;
	
	UAbilitySystemComponent* ASC = MeshComp->GetOwner()->GetComponentByClass<UAbilitySystemComponent>();
	if (!ASC)
		return;

	FGameplayEventData Payload;
	Payload.Instigator = MeshComp->GetOwner();
	Payload.EventTag = CombatTags::Combat_Attack;
	
	FGameplayAbilityTargetingLocationInfo LocationInfo;
	LocationInfo.SourceActor = MeshComp->GetOwner();

	const FVector HitNorm = MeshComp->GetOwner()->GetActorForwardVector();
	const FHitResult OutHit(nullptr, nullptr, MeshComp->GetOwner()->GetActorLocation(), HitNorm);
	Payload.TargetData = LocationInfo.MakeTargetDataHandleFromHitResult(nullptr, OutHit);
	Payload.ContextHandle = ASC->MakeEffectContext();
	
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(MeshComp->GetOwner(),
															 CombatTags::Combat_Attack,
															 Payload);
}
