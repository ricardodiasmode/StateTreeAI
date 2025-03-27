// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <map>

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Misc/GeneralFunctionLibrary.h"
#include "TickManager.generated.h"

class ITickableInterface;

UCLASS()
class COREAS_API ATickManager : public AActor
{
	GENERATED_BODY()

	// fuck unreal cant use tarray inside tmap
	std::map<float, TArray<AActor*>> TickablesTime;

	// fuck unreal cant use tarray inside tmap
	std::map<TArray<AActor*>*, FTimerHandle> TickablesHandle;

	TArray<FTimerHandle> Handles;
	
public:
	// Sets default values for this actor's properties
	ATickManager();

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	void AddTickable(AActor* Tickable, const float Time);
	void RemoveTickable(AActor* Tickable, const float Time);

	static ATickManager* GetTickManager(UWorld* Context);
};
