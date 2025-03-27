// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "NavComponentAS.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NAVIGATIONAS_API UNavComponentAS : public UActorComponent
{
	GENERATED_BODY()

private:
	FCriticalSection GenerationMutex;

	UPROPERTY()
	AActor* CurrentTarget = nullptr;
	FVector TargetLocation;
	
	UPROPERTY()
	ACharacter* OwnerCharacter = nullptr;

	TArray<FVector> CurrentGraph;
	
	TArray<TArray<int32>> GraphAdjacency;

	TArray<FVector> CurrentDesiredPath;
	TArray<FVector> UpdatedDesiredPath;
	bool PathDefined = false;
	
	float CurrentAcceptanceRadius = 15.f;

	FTimerHandle DeadlineHandle;

	bool EndPlayReceived = false;

protected:
	UPROPERTY(EditDefaultsOnly, Category="Config|Generation")
	float NodesDist = 90.f;

	UPROPERTY(EditDefaultsOnly, Category="Config|Generation")
	float MaxForwardDist = 900.f;

	UPROPERTY(EditDefaultsOnly, Category="Config|Generation")
	float MaxBackwardDist = 900.f;
	
	// Symmetric if multiple of NodesDist
	UPROPERTY(EditDefaultsOnly, Category="Config|Generation")
	float MaxRightDist = 4500.f;
	
	// Symmetric if multiple of NodesDist
	UPROPERTY(EditDefaultsOnly, Category="Config|Generation")
	float MaxUpDist = 4000.f;
	
	// So we avoid hitting ground
	UPROPERTY(EditDefaultsOnly, Category="Config|Generation")
	float GraphGenerationCheckHeightOffset = 10.f;
	
	UPROPERTY(EditDefaultsOnly, Category="Config|Generation")
	float GraphGenerationInterval = 2.f;
	
	UPROPERTY(EditDefaultsOnly, Category="Config|Generation")
	float CollisionTolerance = 2.f;

	UPROPERTY(EditDefaultsOnly, Category="Config|PathFinding")
	int MaxClosestNavigationPointsToCheckToRemove = 3;
	
	UPROPERTY(EditDefaultsOnly, Category="Config|PathFinding")
	float FollowUpdateInterval = 0.5f;

	UPROPERTY(EditDefaultsOnly, Category="Config|Debug")
	float DebugGraphInterval = 1.f;
	
	FTimerHandle DebugGraphHandle;
	UPROPERTY(EditDefaultsOnly, Category="Config|Debug")
	uint8 bDebugGraph : 1 = false;
	
	UPROPERTY(EditDefaultsOnly, Category="Config|Debug")
	uint8 bDebugWalk : 1 = false;
	
	UPROPERTY(EditDefaultsOnly, Category="Config|Debug")
	uint8 bDebugPathFinding : 1 = false;

private:
	int GetClosestPointOnPathToOwner(const TArray<FVector>& VectorPath);

	int GetFarthestReachablePoint(const TArray<FVector>& VectorPath);

	void StartWalking();

	void Walk(const float DeltaTime);

	void Debug();

	void OnReachDeadline();

protected:
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void BeginDestroy() override;

public:
	// Sets default values for this component's properties
	UNavComponentAS();
	
	void GenerateGraph();

	void FindPathToClosestPointOnGraph();

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void WalkToActor(AActor* Target,
	const float AcceptanceRadius = 5.f,
		const float Deadline = 15.f);
	void StartDeadlineTimer(float Deadline);

	UFUNCTION(BlueprintCallable)
	void WalkToLocation(const FVector& Target,
		const float AcceptanceRadius = 15.f,
		const float Deadline = 15.f);
	
	UFUNCTION(BlueprintCallable)
	void StopWalking();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMoveSuccessSignature);
	UPROPERTY(BlueprintAssignable)
	FOnMoveSuccessSignature OnMoveSuccess;
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMoveFailedSignature);
	UPROPERTY(BlueprintAssignable)
	FOnMoveFailedSignature OnMoveFailed;

	friend struct FNavThreadManager;
};
