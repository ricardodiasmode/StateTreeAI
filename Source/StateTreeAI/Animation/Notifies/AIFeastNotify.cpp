// Fill out your copyright notice in the Description page of Project Settings.


#include "AIFeastNotify.h"

#include "StateTreeAI/GameFramework/AI/BaseAICharacter.h"

void UAIFeastNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                             const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp)
		return;
	if (!MeshComp->GetOwner())
		return;
	
	ABaseAICharacter* BaseAICharacter = MeshComp->GetOwner<ABaseAICharacter>();
	if (!BaseAICharacter)
		return;
	
	BaseAICharacter->OnFeast();
}
