#include "NavThreadManager.h"

#include "Components/NavComponentAS.h"
#include "Kismet/KismetSystemLibrary.h"

FRunnableThread* FNavThreadManager::GraphThread;
FGraphGeneratorRunnable* FNavThreadManager::GraphGenerator;
FRunnableThread* FNavThreadManager::PathThread = nullptr;
FPathFinderRunnable* FNavThreadManager::PathFinder = nullptr;
FCriticalSection FNavThreadManager::GraphMutex;
TArray<TWeakObjectPtr<UNavComponentAS>> FNavThreadManager::GraphComponents;
TArray<TWeakObjectPtr<UNavComponentAS>> FNavThreadManager::PathComponents;
FCriticalSection FNavThreadManager::PathMutex;

void FNavThreadManager::AddComponent(UNavComponentAS* Component)
{
    TRACE_CPUPROFILER_EVENT_SCOPE_STR("AddComponent");
	{
		FScopeLock GraphLock(&GraphMutex);
		if (GraphComponents.Num() == 0)
		{
			GraphGenerator = new FGraphGeneratorRunnable(Component->GraphGenerationInterval);
			GraphThread = FRunnableThread::Create(GraphGenerator, TEXT("GraphGeneratorThread"));
		}
		GraphComponents.AddUnique(Component);
	}
	{
		FScopeLock PathLock(&PathMutex);
		if (PathComponents.Num() == 0 && !PathThread)
		{
			PathFinder = new FPathFinderRunnable(Component->FollowUpdateInterval);
			PathThread = FRunnableThread::Create(PathFinder, TEXT("PathFinderThread"));
		}
		PathComponents.AddUnique(Component);
	}
}

void FNavThreadManager::RemoveComponent(UNavComponentAS* Component)
{
    TRACE_CPUPROFILER_EVENT_SCOPE_STR("RemoveComponent");
	{
		FScopeLock GraphLock(&GraphMutex);
		GraphComponents.Remove(Component);
	}
	{
		FScopeLock PathLock(&PathMutex);
		PathComponents.Remove(Component);
	}
}