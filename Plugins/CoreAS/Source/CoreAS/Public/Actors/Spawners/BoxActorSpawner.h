// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Misc/GeneralFunctionLibrary.h"
#include "BoxActorSpawner.generated.h"

struct SpawnerInfo
{
	FVector Location;
	TSubclassOf<AActor> Class;

	bool operator==(const SpawnerInfo& other) const
	{
		return other.Location.Equals(Location, 5.f) &&
			other.Class == Class;
	}
};

UCLASS()
class COREAS_API ABoxActorSpawner : public AActor
{
	GENERATED_BODY()
private:
	FTimerHandle RespawnTimerHandle;

	// To use on pooling
	UPROPERTY()
	TMap<TSubclassOf<AActor>, FStructActorArray> AvailableObjects;

protected:
	int SpawnedActors = 0;
	
	UPROPERTY(EditDefaultsOnly)
	class UBillboardComponent* BillboardRoot = nullptr;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* SpawnBox = nullptr;

public:	
	TArray<SpawnerInfo> LocationsToSpawn;
	
	UPROPERTY(EditAnywhere, Category="Config")
	bool bActivatePooling = false;
	
	UPROPERTY(EditAnywhere, Category="Config")
	TArray<TSubclassOf<AActor>> ClassesToSpawn;
	
	UPROPERTY(EditAnywhere, Category="Config")
	int MaxActorsSpawned = 1;
	
	UPROPERTY(EditAnywhere, Category="Config")
	FVector2D RespawnDeviationUntilGetsFull = FVector2D(2.f, 10.f);
	
	UPROPERTY(EditAnywhere, Category="Config")
	TEnumAsByte<ECollisionChannel> NotCollideWith;
	
	UPROPERTY(EditAnywhere, Category="Config")
	TEnumAsByte<EObjectTypeQuery> WorldChannel;
	
	UPROPERTY(EditAnywhere, Category="Config")
	int SpawnAttemptsBeforeFail = 20;
	
	UPROPERTY(EditAnywhere, Category="Config")
	float ActorSpawnHeightOffset = 90.f;
	
	UPROPERTY(EditAnywhere, Category="Config")
	float CollisionRadius = 90.f;
	
	UPROPERTY(EditAnywhere, Category="Config")
	uint8 bDebugSpawn : 1 = false;
	
	UPROPERTY(EditAnywhere, Category="Config")
	uint8 bScheduleRespawnAtStart : 1 = false;

private:
	void AddToObjectPool(AActor* ObjectToAdd);

protected:
	void RespawnActor();
	
	virtual void ScheduleRespawn();

	virtual void StopRespawnTimer();
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
public:	
	// Sets default values for this actor's properties
	ABoxActorSpawner();
	
	bool GetRandomSpawnLocation(FVector& ReturnLocation);

	void OnActorDie(AActor* ObjectToAdd);
};
