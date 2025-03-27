// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnChecker.h"

#include "Actors/Spawners/BoxActorSpawner.h"
#include "Actors/Spawners/SpawnerManager.h"
#include "Kismet/GameplayStatics.h"


// Sets default values for this component's properties
USpawnChecker::USpawnChecker()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

// Called when the game starts
void USpawnChecker::BeginPlay()
{
	Super::BeginPlay();

	StartChecking();
}

void USpawnChecker::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}

TArray<AActor*> USpawnChecker::SortByOwnerDistance(const TArray<AActor*>& Spawners)
{
	TArray<AActor*> SortedSpawners = Spawners;

	// Get the location of the owner of the SpawnChecker.
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		// If no owner is found, handle appropriately (e.g., early exit).
		return SortedSpawners;
	}
	const FVector OwnerLocation = Owner->GetActorLocation();

	// Sort spawners by distance to the owner's location.
	SortedSpawners.Sort([OwnerLocation](const AActor& A, const AActor& B)
	{
		float DistA = FVector::Dist(A.GetActorLocation(), OwnerLocation);
		float DistB = FVector::Dist(B.GetActorLocation(), OwnerLocation);
		return DistA < DistB;
	});

	return SortedSpawners;
}

void USpawnChecker::CheckShouldSpawn()
{
	TArray<AActor*> Spawners;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(),
		ABoxActorSpawner::StaticClass(),
		Spawners);

	TArray<AActor*> SortedSpawners = SortByOwnerDistance(Spawners);

	for (int i = 0; i < MaxSpawnersToCheck && i < SortedSpawners.Num(); i++)
	{
		ABoxActorSpawner* Spawner = Cast<ABoxActorSpawner>(SortedSpawners[i]);
		int j = 0;
		while (j < Spawner->LocationsToSpawn.Num())
		{
			const SpawnerInfo& Info = Spawner->LocationsToSpawn[j];
			if (FVector::Dist(GetOwner()->GetActorLocation(), Info.Location) < DistanceToSpawn)
			{
				Spawner->LocationsToSpawn.RemoveAt(j);

				ASpawnerManager::GetSpawnerManager(GetWorld())->AddToQueue(
					{
						Spawner,
						Info.Class,
					Info.Location
					});

				if (bDebug)
				{
					DrawDebugLine(GetWorld(),
						Info.Location,
						Info.Location + FVector(0.f, 0.f, 5000.f),
						FColor::Green,
						false,
						60.f,
						0,
						30.f);
				}
				
				j--;
			}
			j++;
		}
	}
}

void USpawnChecker::StartChecking()
{
	GetWorld()->GetTimerManager().SetTimer(CheckHandle,
		this,
		&USpawnChecker::CheckShouldSpawn,
		CheckDelay,
		true);
}
