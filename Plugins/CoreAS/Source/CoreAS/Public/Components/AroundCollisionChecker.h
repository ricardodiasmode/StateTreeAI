// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/ObjectToCheckCollisionInterface.h"
#include "AroundCollisionChecker.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class COREAS_API UAroundCollisionChecker : public UActorComponent,
	public ICollisionCheckerInterface
{
	GENERATED_BODY()
	
private:
	FTimerHandle CollisionHandle;
	void ScheduleCollisionCheck();

	UPROPERTY()
	TArray<AActor*> CloseObjectsObjects;

protected:
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "Config")
	float CheckInterval = 3.f;
	
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "Config")
	float ObjectsCheckDistance = 6000.f;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "Config")
	TEnumAsByte<ECollisionChannel> TypeToCheck = ECC_GameTraceChannel7;

private:
	UFUNCTION(Server, reliable)
	void Server_AddOrRemoveObjActivator(AActor* Obj,
		const bool Add);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// Sets default values for this component's properties
	UAroundCollisionChecker();
	
	virtual void RemoveObj(AActor* ObjToRemove) override;	
	UFUNCTION(Client, reliable)
	void Client_RemoveObj(AActor* ObjToRemove);	
	
	void CheckObjectCollision();
};
