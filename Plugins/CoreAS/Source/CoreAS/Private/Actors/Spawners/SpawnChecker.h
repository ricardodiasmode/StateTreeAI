// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SpawnChecker.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class COREAS_API USpawnChecker : public UActorComponent
{
	GENERATED_BODY()

	FTimerHandle CheckHandle;

protected:
	UPROPERTY(EditDefaultsOnly, Category="Config")
	float CheckDelay = 5.f;
	
	UPROPERTY(EditDefaultsOnly, Category="Config")
	int MaxSpawnersToCheck = 4;
	
	UPROPERTY(EditDefaultsOnly, Category="Config")
	float DistanceToSpawn = 6000.f;
	
	UPROPERTY(EditDefaultsOnly, Category="Config")
	bool bDebug = false;

private:
	void CheckShouldSpawn();

	TArray<AActor*> SortByOwnerDistance(const TArray<AActor*>& Spawners);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// Sets default values for this component's properties
	USpawnChecker();

	void StartChecking();
	
};
