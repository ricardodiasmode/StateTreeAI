// Fill out your copyright notice in the Description page of Project Settings.
#include "Components/NavComponentAS.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"
#include "NavigationAS/NavThreadManager.h"
#include "Runnables/GraphGeneratorRunnable.h"
#include "Runnables/PathFinderRunnable.h"

// Sets default values for this component's properties
UNavComponentAS::UNavComponentAS()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

    SetComponentTickEnabled(false);
}

void UNavComponentAS::TickComponent(float DeltaTime, enum ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    Walk(DeltaTime);
}

void UNavComponentAS::BeginPlay()
{
	Super::BeginPlay();

	// Owner must be a character
	OwnerCharacter = CastChecked<ACharacter>(GetOwner());

    // Target loc default is owner loc
    TargetLocation.store(GetOwner()->GetActorLocation());

    if (bDebugGraph)
    {
        GetWorld()->GetTimerManager().SetTimer(DebugGraphHandle,
            this,
            &UNavComponentAS::Debug,
            DebugGraphInterval,
            true);
    }
}

void UNavComponentAS::Debug()
{
    FScopeLock GenerationLock(&GenerationMutex);

    for (const FVector& CurrentGraphPos : CurrentGraph)
        DrawDebugSphere(GetWorld(),
            CurrentGraphPos,
            6.f,
            4,
            FColor::Black,
            false,
            DebugGraphInterval);
}

void UNavComponentAS::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

    GetWorld()->GetTimerManager().ClearAllTimersForObject(this);

    EndPlayReceived = true;
}

void UNavComponentAS::BeginDestroy()
{
    Super::BeginDestroy();

    FNavThreadManager::RemoveComponent(this);
}

void UNavComponentAS::WalkToActor(AActor* Target,
    const float AcceptanceRadius,
    const float Deadline)
{
    CurrentAcceptanceRadius = AcceptanceRadius;

    if (Deadline != -1)
        StartDeadlineTimer(Deadline);
    
    CurrentTarget.store(Target);
    
    StartWalking();
}

void UNavComponentAS::OnReachDeadline()
{
    if (bDebugWalk)
        UKismetSystemLibrary::PrintString(GetWorld(), "Deadline reached.");

    OnMoveFailed.Broadcast();
    StopWalking();
}

void UNavComponentAS::StartDeadlineTimer(const float Deadline)
{
    GetWorld()->GetTimerManager().ClearTimer(DeadlineHandle);
    GetWorld()->GetTimerManager().SetTimer(DeadlineHandle,
                                           this,
                                           &UNavComponentAS::OnReachDeadline,
                                           Deadline,
                                           false);
}

void UNavComponentAS::WalkToLocation(const FVector& Target,
                                     const float AcceptanceRadius,
                                     const float Deadline)
{
    CurrentAcceptanceRadius = AcceptanceRadius;

    if (Deadline != -1)
        StartDeadlineTimer(Deadline);
    
    TargetLocation.store(Target);
    
    StartWalking();
}

void UNavComponentAS::StopWalking()
{
    TRACE_CPUPROFILER_EVENT_SCOPE_STR("StopWalking");
    
    GetWorld()->GetTimerManager().ClearTimer(DeadlineHandle);

    SetComponentTickEnabled(false);
    
    FNavThreadManager::RemoveComponent(this);
    
    CurrentTarget.store(nullptr);
    if (GetOwner())
        TargetLocation.store(GetOwner()->GetActorLocation());
}

void UNavComponentAS::StartWalking()
{
    TRACE_CPUPROFILER_EVENT_SCOPE_STR("StartWalking");
    
    PathDefined = false;
    
    FNavThreadManager::AddComponent(this);
    SetComponentTickEnabled(true);
}

void UNavComponentAS::GenerateGraph()
{
    FScopeLock GenerationLock(&GenerationMutex);
    
    if (bDebugGraph)
        UE_LOG(LogTemp, Display, TEXT("Generating graph..."));

    TRACE_CPUPROFILER_EVENT_SCOPE_STR("GenerateGraph");

    // Clear previous data if necessary.
    CurrentGraph.Reset();
    GraphAdjacency.Reset();

    const float OwnerHeight = OwnerCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
    const float OwnerRadius = OwnerCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius();
    const FVector OwnerLoc = OwnerCharacter->GetActorLocation();
    FVector TargetLoc = CurrentTarget.load() ? CurrentTarget.load()->GetActorLocation() : TargetLocation.load();

    // This struct and TArray hold edges (if you need to keep them).
    struct FGraphEdge { int32 FromNode; int32 ToNode; };
    // GraphEdges will be used to store bidirectional edges.
    
    // Map grid coordinates to node index for quick lookup.
    TMap<FIntVector, int32> NodeIndexMap;

    float SnappedForward = FMath::GridSnap(MaxForwardDist, NodesDist);
    float SnappedBack = FMath::GridSnap(MaxBackwardDist, NodesDist);
    float SnappedRight = FMath::GridSnap(MaxRightDist, NodesDist);
    float SnappedUp = FMath::GridSnap(MaxUpDist, NodesDist);

    const FVector ForwardDir = (TargetLoc - OwnerLoc)/(TargetLoc - OwnerLoc).Length();
    const FVector RightDir = FVector::CrossProduct(ForwardDir, FVector(0.f, 0.f, 1.f));
    
    // Iterate through the grid.
    for (float X = -SnappedBack; X < SnappedForward; X += NodesDist)
    {
        for (float Y = -SnappedRight; Y < SnappedRight; Y += NodesDist)
        {
            for (float Z = -SnappedUp; Z < SnappedUp; Z += NodesDist)
            {
                const FVector CurrentPos = OwnerLoc + ForwardDir*X + RightDir*Y + FVector(0.f, 0.f, Z);

                //Collision check to see if the location is valid.
                const bool bInvalidLoc = GetWorld()->SweepTestByChannel(
                    CurrentPos,
                    CurrentPos + FVector(0.1f),
                    FRotator().Quaternion(),
                    ECC_Visibility,
                    FCollisionShape::MakeCapsule(OwnerRadius - CollisionTolerance, OwnerHeight - CollisionTolerance)
                );
                
                if (bInvalidLoc)
                    continue;

                // Add the node to the graph.
                const int32 NewIndex = CurrentGraph.Add(CurrentPos);
                // Also, initialize an empty list of neighbors for this new node.
                GraphAdjacency.AddDefaulted();

                // Compute grid coordinate relative to OwnerLoc.
                const int32 XIndex = FMath::RoundToInt((CurrentPos.X - OwnerLoc.X) / NodesDist);
                const int32 YIndex = FMath::RoundToInt((CurrentPos.Y - OwnerLoc.Y) / NodesDist);
                const int32 ZIndex = FMath::RoundToInt((CurrentPos.Z - OwnerLoc.Z) / NodesDist);
                const FIntVector GridCoord(XIndex, YIndex, ZIndex);

                // Check for already-created neighbors.
                TArray<FIntVector> AllOffsets = {
                    FIntVector(-1, 0, 0), FIntVector(1, 0, 0), // Forward/back
                    FIntVector(0, -1, 0), FIntVector(0, 1, 0), // Left/right
                    FIntVector(0, 0, -1), FIntVector(0, 0, 1)  // Down/up
                };
                for (const FIntVector& Offset : AllOffsets)
                {
                    FIntVector NeighborCoord = GridCoord + Offset;
                    if (int32* NeighborIndexPtr = NodeIndexMap.Find(NeighborCoord))
                    {
                        const int32 NeighborIndex = *NeighborIndexPtr;
                        GraphAdjacency[NewIndex].Add(NeighborIndex);
                        GraphAdjacency[NeighborIndex].Add(NewIndex);
                    }
                }

                // Store the current node in the map using its grid coordinate.
                NodeIndexMap.Add(GridCoord, NewIndex);
            }
        }
    }

    if (bDebugGraph)
        UE_LOG(LogTemp, Display, TEXT("Graph generated with %d nodes"), CurrentGraph.Num());
}

int UNavComponentAS::GetClosestPointOnPathToOwner(const TArray<FVector>& VectorPath)
{
    if (!GetOwner())
        return -1;
    
    const FVector& OwnerLoc = GetOwner()->GetActorLocation();
    int ClosestPoint = -1;
    for (int i = 0; i < VectorPath.Num() && i > MaxClosestNavigationPointsToCheckToRemove; i++)
    {
        if (ClosestPoint == -1 ||
            FVector::Dist(OwnerLoc, VectorPath[i]) < FVector::Dist(OwnerLoc, VectorPath[ClosestPoint]))
            ClosestPoint = i;
    }
    return ClosestPoint;
}

int UNavComponentAS::GetFarthestReachablePoint(const TArray<FVector>& VectorPath)
{
    if (!OwnerCharacter)
        return -1;
    
    const float OwnerHeight = OwnerCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
    const float OwnerRadius = OwnerCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius();
    
    const FVector& OwnerLoc = GetOwner()->GetActorLocation();
    int ClosestPoint = -1;
    for (int i = VectorPath.Num() - 1; i >= 0; i--)
    {
        FCollisionQueryParams QueryParams = FCollisionQueryParams::DefaultQueryParam;
        QueryParams.AddIgnoredActor(GetOwner());

        if (CurrentTarget.load())
            QueryParams.AddIgnoredActor(CurrentTarget.load());
        
        if (!GetWorld()->SweepTestByChannel(OwnerLoc,
            VectorPath[i],
            FQuat::Identity,
            ECC_Visibility,
            FCollisionShape::MakeCapsule(OwnerRadius - CollisionTolerance, OwnerHeight - CollisionTolerance),
            QueryParams))
        {
            ClosestPoint = i;
            break;
        }
    }
    return ClosestPoint;
}

void UNavComponentAS::FindPathToClosestPointOnGraph()
{
    FScopeLock Lock(&GenerationMutex);

    if (bDebugPathFinding)
        UE_LOG(LogTemp, Display, TEXT("FindPathToClosestPointOnGraph..."));
    
    TRACE_CPUPROFILER_EVENT_SCOPE_STR("FindPathToClosestPointOnGraph");
    
    // Get owner's current location.
    const FVector& OwnerLoc = OwnerCharacter->GetActorLocation();
    
    // Find the node in CurrentGraph that is closest to the owner; this is our start node.
    int StartIndex = -1;
    float MinDistToOwner = FLT_MAX;
    for (int i = 0; i < CurrentGraph.Num(); i++)
    {
        float Dist = FVector::Dist(CurrentGraph[i], OwnerLoc);
        if (Dist < MinDistToOwner)
        {
            MinDistToOwner = Dist;
            StartIndex = i;
        }
    }
    
    // If no starting node is found, return an empty path.
    if (StartIndex == -1)
    {
        if (bDebugPathFinding)
            UE_LOG(LogTemp, Display, TEXT("StartIndex == -1 on FindPathToClosestPointOnGraph. Cancelling..."));
        return;
    }
    
    // Define a structure for our A* nodes.
    struct FAStarNode
    {
        int32 Index;
        float G; // Cost from start to this node.
        float F; // Estimated total cost = G + H.
        
        // Overload operator< to make the heap work as a min-heap (by F value).
        bool operator<(const FAStarNode& Other) const
        {
            return F > Other.F; // TArray heap functions create a max-heap by default.
        }
    };
    
    // OpenSet implemented as a heap on a TArray.
    TArray<FAStarNode> OpenSet;
    // Map to track the best g score found so far.
    TMap<int32, float> GScore;
    // Map to track the parent of each node for path reconstruction.
    TMap<int32, int32> Parent;
    
    FVector TargetLoc = CurrentTarget.load() ? CurrentTarget.load()->GetActorLocation() : TargetLocation.load();
    
    auto Heuristic = [&](int32 NodeIndex) -> float
    {
        return FVector::Dist(CurrentGraph[NodeIndex], TargetLoc);
    };
    
    // Initialize the start node.
    FAStarNode StartNode;
    StartNode.Index = StartIndex;
    StartNode.G = 0.0f;
    StartNode.F = Heuristic(StartIndex);
    OpenSet.HeapPush(StartNode);
    
    GScore.Add(StartIndex, 0.0f);
    Parent.Add(StartIndex, -1);
    
    // Keep track of the best node (closest to the target) encountered during the search.
    int BestIndex = StartIndex;
    float BestH = Heuristic(StartIndex);

    // Main A* loop.
    if (bDebugPathFinding)
        UE_LOG(LogTemp, Display, TEXT("Starting A*..."));
    while (OpenSet.Num() > 0)
    {
        FAStarNode CurrentNode;
        OpenSet.HeapPop(CurrentNode);
        int32 CurrentIndex = CurrentNode.Index;
        
        // Check if this node is closer to the target.
        float CurrentH = Heuristic(CurrentIndex);
        if (CurrentH < BestH)
        {
            BestH = CurrentH;
            BestIndex = CurrentIndex;
        }
        
        // Expand neighbors.
        if (!GraphAdjacency.IsValidIndex(CurrentIndex))
            break;
        for (int32 NeighborIndex : GraphAdjacency[CurrentIndex])
        {
            if (!CurrentGraph.IsValidIndex(NeighborIndex) ||
                !CurrentGraph.IsValidIndex(CurrentIndex))
                continue;
            // Calculate cost to reach neighbor.
            float TentativeG = CurrentNode.G + FVector::Dist(CurrentGraph[CurrentIndex], CurrentGraph[NeighborIndex]);
            if (!GScore.Contains(NeighborIndex) || TentativeG < GScore[NeighborIndex])
            {
                GScore.Add(NeighborIndex, TentativeG);
                Parent.Add(NeighborIndex, CurrentIndex);
                FAStarNode NeighborNode;
                NeighborNode.Index = NeighborIndex;
                NeighborNode.G = TentativeG;
                NeighborNode.F = TentativeG + Heuristic(NeighborIndex);
                OpenSet.HeapPush(NeighborNode);
            }
        }
    }
    if (bDebugPathFinding)
        UE_LOG(LogTemp, Display, TEXT("Finished A*..."));
    
    // Reconstruct path from BestIndex back to StartIndex.
    TArray<int32> Path;
    int32 Current = BestIndex;
    while (Current != -1)
    {
        Path.Add(Current);
        if (!Parent.Contains(Current))
        {
            break;
        }
        Current = Parent[Current];
    }
    
    if (bDebugPathFinding)
        UE_LOG(LogTemp, Display, TEXT("A* found %d nodes."), Path.Num());

    // Getting graph as FVector and the Path's reverse.
    TArray<FVector> VectorPath;
    for (int i = Path.Num()-1; i >= 0; i--)
        VectorPath.Add(CurrentGraph[Path[i]]);
    
    if (bDebugPathFinding)
        UE_LOG(LogTemp, Display, TEXT("VectorPath has %d nodes before getting closest point."), VectorPath.Num());

    // Removing points that are behind the closest
    const int ClosestPoint = GetClosestPointOnPathToOwner(VectorPath);
    for (int i = 0; i < ClosestPoint; i++)
        VectorPath.RemoveAt(0);
    
    if (bDebugPathFinding)
        UE_LOG(LogTemp, Display, TEXT("VectorPath has %d nodes before getting farthest point."), VectorPath.Num());

    // Removing points that are behind the farthest that we can reach
    const int FarthestReachablePoint = GetFarthestReachablePoint(VectorPath);
    for (int i = 0; i < FarthestReachablePoint; i++)
        VectorPath.RemoveAt(0);
    
    if (bDebugPathFinding)
        UE_LOG(LogTemp, Display, TEXT("VectorPath finally has %d nodes after pathfinding."), VectorPath.Num());

    TWeakObjectPtr<UNavComponentAS> WeakThis = this;
    AsyncTask(ENamedThreads::GameThread, [WeakThis, VectorPath]()
    {
        if (WeakThis.IsValid(false, true) &&
            WeakThis.Get())
        {
            WeakThis->UpdatedDesiredPath = VectorPath;
            WeakThis->PathDefined = true;
        }
    });
}

void UNavComponentAS::Walk(const float DeltaTime)
{
    if (UpdatedDesiredPath != CurrentDesiredPath)
        CurrentDesiredPath = UpdatedDesiredPath;

    if (!PathDefined)
        return;
    
    if (!OwnerCharacter)
    {
        OnMoveFailed.Broadcast();
        StopWalking();
        return;
    }

    const FVector LocationToCompare = CurrentTarget.load() ? CurrentTarget.load()->GetActorLocation() : TargetLocation.load();
    if (FVector::Dist(LocationToCompare, OwnerCharacter->GetActorLocation()) <= CurrentAcceptanceRadius)
    {
        if (bDebugWalk)
        {
            DrawDebugSphere(GetWorld(),
                OwnerCharacter->GetActorLocation(),
                24.f,
                4,
                FColor::Green,
                false,
                DeltaTime+1.f,
                0,
                4.f);
        }
        OnMoveSuccess.Broadcast();
        StopWalking();
        return;
    }
    
    if (CurrentDesiredPath.IsEmpty() && PathDefined)
    {
        if (bDebugWalk)
        {
            DrawDebugSphere(GetWorld(),
                OwnerCharacter->GetActorLocation(),
                24.f,
                4,
                FColor::Red,
                false,
                DeltaTime+1.f,
                0,
                4.f);
        }
        OnMoveFailed.Broadcast();
        StopWalking();
        return;
    }

    const FVector OwnerLoc = OwnerCharacter->GetActorLocation();
    const FVector2D OwnerLoc2D(OwnerLoc.X, OwnerLoc.Y);
    const FVector2D NextPathLoc2D(CurrentDesiredPath[0].X, CurrentDesiredPath[0].Y);
    if (FVector2D::Distance(OwnerLoc2D, NextPathLoc2D) <= CurrentAcceptanceRadius)
    {
        if (bDebugWalk)
        {
            DrawDebugSphere(GetWorld(),
                CurrentDesiredPath[0],
                24.f,
                4,
                FColor::Blue,
                false,
                DeltaTime+1.f,
                0,
                4.f);
        }
        CurrentDesiredPath.RemoveAt(0);
    }
    if (CurrentDesiredPath.IsEmpty())
    {
        OnMoveSuccess.Broadcast();
        StopWalking();
        return;
    }
    
    FVector MovementDirection = CurrentDesiredPath[0] - OwnerLoc;
    MovementDirection.Z = 0.f;
    MovementDirection.Normalize();
    OwnerCharacter->AddMovementInput(MovementDirection);

    if (bDebugWalk)
    {
        DrawDebugDirectionalArrow(GetWorld(),
           OwnerLoc,
           OwnerLoc + MovementDirection*50.f,
           50.f,
           FColor::Red,
           false,
           DeltaTime+.01f,
           0,
           8.f);

        for (const FVector& CurrentPoint : CurrentDesiredPath)
            DrawDebugSphere(GetWorld(),
                CurrentPoint,
                12.f,
                4,
                FColor::Green,
                false,
                DeltaTime+.01f,
                0,
                4.f);
    }
}
