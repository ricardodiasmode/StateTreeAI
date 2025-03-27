// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PatrolActor.generated.h"

UCLASS()
class STATETREEAI_API APatrolActor : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly)
	USceneComponent* SceneRoot = nullptr;
	
	UPROPERTY(EditAnywhere)
	TArray<UBillboardComponent*> PatrolPoints;
	
	UPROPERTY(EditAnywhere, Category="Config", meta=(ClampMin=0))
	int NumberOfPoints = 0;

	UPROPERTY(EditAnywhere, Category="Config")
	UTexture2D* BillboardSprite = nullptr;

private:
	void RemoveExceedingPatrolPoints();
	void AddRemainingPatrolPoints();
	void CheckPatrolPoints();

public:
	// Sets default values for this actor's properties
	APatrolActor();

	UFUNCTION(CallInEditor, Category="Config")
	void FlushLines() { FlushPersistentDebugLines(GetWorld()); }
	UFUNCTION(CallInEditor, Category="Config")
	void DebugLines();

	virtual void OnConstruction(const FTransform& Transform) override;

	TArray<FVector> GetPatrolPoints() const;
};
