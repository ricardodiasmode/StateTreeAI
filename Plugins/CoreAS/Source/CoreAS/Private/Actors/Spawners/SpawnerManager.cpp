// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Spawners/SpawnerManager.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ASpawnerManager::ASpawnerManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

void ASpawnerManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorldTimerManager().ClearAllTimersForObject(this);
}

void ASpawnerManager::Spawn()
{
	if (bDebug)
		UKismetSystemLibrary::PrintString(GetWorld(),
			FString::Printf(TEXT("Start spawning. Queue size: %d"), SpawnQueue.Num()));
		
	int SpawnedActors = 0;
	while (SpawnedActors <= MaxSpawnsPerTick && !SpawnQueue.IsEmpty())
	{
		const ActorToSpawn SpawnInfo = SpawnQueue.Pop();

		FActorSpawnParameters Params;
		Params.Owner = SpawnInfo.Owner;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(SpawnInfo.Class,
			SpawnInfo.Loc,
			FRotator(),
			Params);
		SpawnedActor->Owner = SpawnInfo.Owner;
		
		SpawnedActors++;
	}

	if (SpawnQueue.IsEmpty())
		GetWorldTimerManager().ClearTimer(SpawnHandle);

	if (bDebug)
		UKismetSystemLibrary::PrintString(GetWorld(),
			FString::Printf(TEXT("Finish spawn queue. Queue size: %d"), SpawnQueue.Num()));
}

void ASpawnerManager::AddToQueue(const ActorToSpawn& SpawnInfo)
{
	SpawnQueue.Add(SpawnInfo);

	if (GetWorldTimerManager().IsTimerActive(SpawnHandle))
		return;

	GetWorldTimerManager().SetTimer(SpawnHandle,
		this,
		&ASpawnerManager::Spawn,
		SpawnCheckInterval,
		true);
}

ASpawnerManager* ASpawnerManager::GetSpawnerManager(UWorld* Context)
{
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(Context, ASpawnerManager::StaticClass(), OutActors);

	if (OutActors.IsEmpty())
	{
		return Context->SpawnActor<ASpawnerManager>(ASpawnerManager::StaticClass());
	}
	return Cast<ASpawnerManager>(OutActors[0]);
}

