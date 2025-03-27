// Fill out your copyright notice in the Description page of Project Settings.


#include "PatrolActor.h"

#include "Components/BillboardComponent.h"


// Sets default values
APatrolActor::APatrolActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>("SceneRoot");
	RootComponent = SceneRoot;
}

void APatrolActor::AddRemainingPatrolPoints()
{
	while (PatrolPoints.Num() < NumberOfPoints)
	{
		UBillboardComponent* ComponentToAdd = NewObject<UBillboardComponent>(this);
		ComponentToAdd->SetupAttachment(GetRootComponent());
		ComponentToAdd->RegisterComponent();
		AddInstanceComponent(ComponentToAdd);
		PatrolPoints.Add(ComponentToAdd);
	}
}

void APatrolActor::RemoveExceedingPatrolPoints()
{
	while (PatrolPoints.Num() > NumberOfPoints)
	{
		UBillboardComponent* ComponentToDestroy = PatrolPoints.Pop();
		if (IsValid(ComponentToDestroy))
			ComponentToDestroy->DestroyComponent();
	}
}

void APatrolActor::CheckPatrolPoints()
{
	if (PatrolPoints.Num() == NumberOfPoints)
		return;
	
	if (PatrolPoints.Num() > NumberOfPoints)
		RemoveExceedingPatrolPoints();
	else
		AddRemainingPatrolPoints();

	for (UBillboardComponent* Component : PatrolPoints)
		Component->SetSprite(BillboardSprite);
}

void APatrolActor::OnConstruction(const FTransform& Transform)
{
	CheckPatrolPoints();
	
	Super::OnConstruction(Transform);
}

void APatrolActor::DebugLines()
{
	if (PatrolPoints.Num() <= 1)
		return;
	
	for (int i = 0; i < PatrolPoints.Num() - 1; i++)
	{
		const float ArrowSize = FVector::Distance(PatrolPoints[i]->GetComponentLocation(), PatrolPoints[i+1]->GetComponentLocation());
		DrawDebugDirectionalArrow(GetWorld(),
								  PatrolPoints[i]->GetComponentLocation(),
								  PatrolPoints[i+1]->GetComponentLocation(),
								  ArrowSize,
								  FColor::Green,
								  true,
								  -1,
								  0,
								  8.f);
	}

	// Last one to first
	const float ArrowSize = FVector::Distance(PatrolPoints[PatrolPoints.Num()-1]->GetComponentLocation(), PatrolPoints[0]->GetComponentLocation());
	DrawDebugDirectionalArrow(GetWorld(),
	  PatrolPoints[PatrolPoints.Num()-1]->GetComponentLocation(),
	  PatrolPoints[0]->GetComponentLocation(),
	  ArrowSize,
	  FColor::Green,
	  true,
	  -1,
	  0,
	  8.f);
}

TArray<FVector> APatrolActor::GetPatrolPoints() const
{
	TArray<FVector> ReturnVector;
	for (const UBillboardComponent* PatrolPoint : PatrolPoints)
		ReturnVector.Add(PatrolPoint->GetComponentLocation());
	return ReturnVector;
}
