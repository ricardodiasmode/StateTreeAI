// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/AroundCollisionChecker.h"

#include "Interfaces/ObjectToCheckCollisionInterface.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values for this component's properties
UAroundCollisionChecker::UAroundCollisionChecker()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UAroundCollisionChecker::ScheduleCollisionCheck()
{
	if (APawn* OwnerAsPawn = Cast<APawn>(GetOwner()))
	{ // If multiplayer, only schedule on client
		if (!OwnerAsPawn->IsLocallyControlled())
			return;
	}

	GetOwner()->GetWorldTimerManager().SetTimer(CollisionHandle,
									this,
									&UAroundCollisionChecker::CheckObjectCollision,
									CheckInterval,
									true);
}

// Called when the game starts
void UAroundCollisionChecker::BeginPlay()
{
	Super::BeginPlay();

	ScheduleCollisionCheck();	
}

void UAroundCollisionChecker::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (GetOwner()->HasAuthority())
	{ // Only server saves hidden objs
		for (int i = 0; i < CloseObjectsObjects.Num(); i++)
		{
			if (IObjectToCheckCollisionInterface* HiddenObj = Cast<IObjectToCheckCollisionInterface>(CloseObjectsObjects[i]))
				HiddenObj->RemoveActivator(this);
		}
	}
	
	Super::EndPlay(EndPlayReason);
}

void UAroundCollisionChecker::Server_AddOrRemoveObjActivator_Implementation(AActor* Obj,
                                                                                  const bool Add)
{
	if (IObjectToCheckCollisionInterface* ObjInterface = Cast<IObjectToCheckCollisionInterface>(Obj))
	{
		if (Add)
			ObjInterface->AddActivator(this);
		else
			ObjInterface->RemoveActivator(this);
	}
}

void UAroundCollisionChecker::CheckObjectCollision()
{
	TRACE_CPUPROFILER_EVENT_SCOPE_STR("CheckHiddenObjectCollision");
	TArray<FHitResult> OutHits;
	UKismetSystemLibrary::SphereTraceMulti(GetWorld(),
		GetOwner()->GetActorLocation(),
		GetOwner()->GetActorLocation() + 1.f,
		ObjectsCheckDistance,
		UEngineTypes::ConvertToTraceType(TypeToCheck), // HiddenObj channel
		false,
		{GetOwner()},
		EDrawDebugTrace::None,
		OutHits,
		true);

	TArray<AActor*> FoundObj;
	for (const FHitResult& CurrentHit : OutHits)
	{
		FoundObj.Add(CurrentHit.GetActor());

		if (IObjectToCheckCollisionInterface* HiddenObj = Cast<IObjectToCheckCollisionInterface>(CurrentHit.GetActor()))
		{
			if (!CloseObjectsObjects.Contains(CurrentHit.GetActor()))
			{
				Server_AddOrRemoveObjActivator(CurrentHit.GetActor(), true);
				CloseObjectsObjects.AddUnique(CurrentHit.GetActor());
			}
		}
	}

	// Remove Objs that are not close
	int i = 0;
	while(i < CloseObjectsObjects.Num())
	{
		if (!FoundObj.Contains(Cast<AActor>(CloseObjectsObjects[i])))
		{
			Server_AddOrRemoveObjActivator(CloseObjectsObjects[i], false);
			CloseObjectsObjects.RemoveAt(i);
			continue;
		}
		i++;
	}
}

void UAroundCollisionChecker::RemoveObj(AActor* ObjToRemove)
{
	Client_RemoveObj(ObjToRemove);
}

void UAroundCollisionChecker::Client_RemoveObj_Implementation(AActor* ObjToRemove)
{
	CloseObjectsObjects.Remove(ObjToRemove);
}

