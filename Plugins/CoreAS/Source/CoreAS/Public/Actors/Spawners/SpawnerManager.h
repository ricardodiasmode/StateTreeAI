// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnerManager.generated.h"

struct ActorToSpawn
{
	AActor* Owner = nullptr;
	TSubclassOf<AActor> Class;
	FVector Loc;
};

/* Can be used to schedule respawns in a performatic way, but will not respect delays if there are too many spawners.
 * You must use your custom ActorSpawner overriding SpawnActor function to use this manager. 
 */
UCLASS()
class COREAS_API ASpawnerManager : public AActor
{
	GENERATED_BODY()

	TArray<ActorToSpawn> SpawnQueue;

	FTimerHandle SpawnHandle;

protected:
	UPROPERTY(EditDefaultsOnly, Category="Config")
	float SpawnCheckInterval = 1.f;

	UPROPERTY(EditDefaultsOnly, Category="Config")
	int MaxSpawnsPerTick = 10;

	UPROPERTY(EditDefaultsOnly, Category="Config")
	bool bDebug = true;

public:
	// Sets default values for this actor's properties
	ASpawnerManager();

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void Spawn();
	
	void AddToQueue(const ActorToSpawn& SpawnInfo);

	/* todo: we can make a template function to spawn custom spawner managers and define custom variables.
	 * for now idc.
	*/
	static ASpawnerManager* GetSpawnerManager(UWorld* Context);
};
