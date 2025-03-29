// Fill out your copyright notice in the Description page of Project Settings.


#include "AIFinishFeastNotify.h"

#include "Misc/GeneralFunctionLibrary.h"
#include "StateTreeAI/GameFramework/AI/BaseAICharacter.h"

void UAIFinishFeastNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
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
	
	BaseAICharacter->OnFinishFeast();
}
