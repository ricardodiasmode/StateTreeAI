// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/TickManager.h"

#include "Interfaces/TickableInterface.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ATickManager::ATickManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

void ATickManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	for (FTimerHandle Handle : Handles)
		GetWorldTimerManager().ClearTimer(Handle);
}

void ATickManager::AddTickable(AActor* Tickable, const float Time)
{
	if (TickablesTime.contains(Time))
	{
		TickablesTime[Time].Add(Tickable);
	} else
	{
		FTimerHandle Handle;
		Handles.Add(Handle);

		TickablesTime.insert({Time, {Tickable}});

		TickablesHandle.insert({&TickablesTime[Time], Handle});

		FTimerManagerTimerParameters TimerParams;
		TimerParams.bLoop = true;
		TimerParams.bMaxOncePerFrame = true;
		GetWorldTimerManager().SetTimer(Handle,
			[this, Time] {
				int InitialArrayNum = TickablesTime[Time].Num();
				int ArrayIter = 0;
				while (ArrayIter < TickablesTime[Time].Num() && ArrayIter >= 0)
				{
					CastChecked<ITickableInterface>(TickablesTime[Time][ArrayIter])->TickFromManager();

					if (InitialArrayNum > TickablesTime[Time].Num())
					{ // Obj was removed from TickableArray. Expecting max of 1 removal per loop (which is the current position).
						ArrayIter--;
						InitialArrayNum--;
					}

					ArrayIter++;
				}
			},
			Time,
			TimerParams);
	}
}

void ATickManager::RemoveTickable(AActor* Tickable, const float Time)
{
	if (!TickablesTime.contains(Time))
		return;
	
	TickablesTime[Time].Remove(Tickable);

	if (TickablesTime[Time].IsEmpty())
	{ // Clearing stuff if there is no tickable remaining
		if (TickablesHandle.contains(&TickablesTime[Time]))
			GetWorldTimerManager().ClearTimer(TickablesHandle[&TickablesTime[Time]]);
		TickablesTime.erase(Time);
	}
}

ATickManager* ATickManager::GetTickManager(UWorld* Context)
{
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(Context, ATickManager::StaticClass(), OutActors);

	if (OutActors.IsEmpty())
	{
		return Context->SpawnActor<ATickManager>(ATickManager::StaticClass());
	}
	return Cast<ATickManager>(OutActors[0]);
}

