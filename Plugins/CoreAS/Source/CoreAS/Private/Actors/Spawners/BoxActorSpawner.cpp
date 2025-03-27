// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Spawners/BoxActorSpawner.h"

#include "Components/BillboardComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ABoxActorSpawner::ABoxActorSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SpawnBox = CreateDefaultSubobject<UBoxComponent>("SpawnBox");
	SpawnBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SpawnBox->SetComponentTickEnabled(false);
	RootComponent = SpawnBox;

	BillboardRoot = CreateDefaultSubobject<UBillboardComponent>("BillboardRoot");
	BillboardRoot->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ABoxActorSpawner::BeginPlay()
{
	Super::BeginPlay();

	if (bScheduleRespawnAtStart)
		ScheduleRespawn();
}

void ABoxActorSpawner::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorldTimerManager().ClearAllTimersForObject(this);
}

bool ABoxActorSpawner::GetRandomSpawnLocation(FVector& ReturnLocation)
{
	const FVector Extent = SpawnBox->GetScaledBoxExtent();

	int RemainingAttempts = SpawnAttemptsBeforeFail;
	while (RemainingAttempts > 0)
	{
		const FVector RandomLoc = FVector(FMath::RandRange(-Extent.X, Extent.X), FMath::RandRange(-Extent.Y, Extent.Y), GetActorLocation().Z + 10000.f);

		FHitResult WorldHit;
		UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(),
													SpawnBox->GetComponentLocation() + RandomLoc,
													SpawnBox->GetComponentLocation() + RandomLoc - FVector(0.f, 0.f, 50000.f),
													{WorldChannel},
													false,
													{this},
													bDebugSpawn ? EDrawDebugTrace::Persistent : EDrawDebugTrace::None,
													WorldHit,
													true);

		if (WorldHit.bBlockingHit)
		{
			const FVector SpawnLoc = WorldHit.ImpactPoint + FVector(0.f, 0.f, ActorSpawnHeightOffset);

			if (!GetWorld()->SweepTestByChannel(SpawnLoc,
				SpawnLoc + 1.f,
				FQuat::Identity,
				NotCollideWith,
				FCollisionShape::MakeSphere(CollisionRadius)))
			{
				if (bDebugSpawn)
					DrawDebugSphere(GetWorld(),
						SpawnLoc,
						CollisionRadius,
						32,
						FColor::Green,
						true);
				
				ReturnLocation = SpawnLoc;
				return true;
			} else
			{
				if (bDebugSpawn)
					DrawDebugSphere(GetWorld(),
						SpawnLoc,
						CollisionRadius,
						32,
						FColor::Red,
						true);
			}
		}
		
		RemainingAttempts--;
	}
	return false;
}

void ABoxActorSpawner::StopRespawnTimer()
{
	GetWorldTimerManager().ClearTimer(RespawnTimerHandle);
}

void ABoxActorSpawner::RespawnActor()
{
	TRACE_CPUPROFILER_EVENT_SCOPE_STR("RespawnActor");

	if (SpawnedActors >= MaxActorsSpawned ||
		ClassesToSpawn.IsEmpty())
	{
		StopRespawnTimer();
		return;
	}
	
	if (bDebugSpawn)
		GPrintDebug("Trying to respawn...");
	
	TSubclassOf<AActor> RandomClass = ClassesToSpawn[FMath::RandRange(0, ClassesToSpawn.Num() - 1)];
	
	if (AvailableObjects.Contains(RandomClass) && !AvailableObjects[RandomClass].EncapsulatedArray.IsEmpty())
	{
		AActor* AvailableActor = AvailableObjects[RandomClass].EncapsulatedArray.Pop();
		
		if (bDebugSpawn)
			GPrintDebugWithVar("Resetting actor: %s", *UKismetSystemLibrary::GetDisplayName(AvailableActor));
		
		AvailableActor->Reset();
		if (FVector SpawnLoc; GetRandomSpawnLocation(SpawnLoc))
		{
			AvailableActor->SetActorLocation(SpawnLoc);
			
			if (bDebugSpawn)
				DrawDebugSphere(GetWorld(),
					SpawnLoc,
					CollisionRadius,
					10,
					FColor::Black,
					true);
	
			SpawnedActors++;
		}
	} else
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		SpawnParams.Owner = this;
		
		if (FVector SpawnLoc; GetRandomSpawnLocation(SpawnLoc))
		{
			if (bActivatePooling)
			{
				const FTransform SpawnTransform(FRotator(), SpawnLoc, FVector(1.f));
				AActor* SpawnedActor = GetWorld()->SpawnActorDeferred<AActor>(RandomClass,
																	  SpawnTransform);
	
				SpawnedActor->Owner = this; // just reforcing
			} else
			{
				if (bDebugSpawn)
					GPrintDebug("Adding loc on locations to spawn.");
				LocationsToSpawn.Add({SpawnLoc, RandomClass});
			}

			SpawnedActors++;
		}
	}
}

void ABoxActorSpawner::ScheduleRespawn()
{
	if (SpawnedActors >= MaxActorsSpawned ||
		ClassesToSpawn.IsEmpty())
		return;
	
	if (bDebugSpawn)
		GPrintDebug("Respawn scheduled.");
	
	GetWorldTimerManager().SetTimer(RespawnTimerHandle,
		this,
		&ABoxActorSpawner::RespawnActor,
		FMath::RandRange(RespawnDeviationUntilGetsFull.X, RespawnDeviationUntilGetsFull.Y),
		true);
}

void ABoxActorSpawner::AddToObjectPool(AActor* ObjectToAdd)
{
	if (AvailableObjects.Contains(ObjectToAdd->StaticClass()))
		AvailableObjects[ObjectToAdd->StaticClass()].EncapsulatedArray.Add(ObjectToAdd);
	else
		AvailableObjects.Add(ObjectToAdd->GetClass(), FStructActorArray(ObjectToAdd));
}

void ABoxActorSpawner::OnActorDie(AActor* ObjectToAdd)
{
	if (bDebugSpawn)
		GPrintDebug("Adding object to pool.");
	
	SpawnedActors--;

	if (bActivatePooling)
		AddToObjectPool(ObjectToAdd);

	ScheduleRespawn();
}
