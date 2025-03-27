#include "NavThreadManager.h"

#include "Components/NavComponentAS.h"
#include "Kismet/KismetSystemLibrary.h"

FRunnableThread* FNavThreadManager::GraphThread = nullptr;
FRunnableThread* FNavThreadManager::PathThread = nullptr;
FGraphGeneratorRunnable* FNavThreadManager::GraphGenerator = nullptr;
FPathFinderRunnable* FNavThreadManager::PathFinder = nullptr;
FCriticalSection FNavThreadManager::GraphMutex;
TArray<TWeakObjectPtr<UNavComponentAS>> FNavThreadManager::GraphComponents;
TArray<TWeakObjectPtr<UNavComponentAS>> FNavThreadManager::PathComponents;
FCriticalSection FNavThreadManager::PathMutex;
FCriticalSection FNavThreadManager::TargetMutex;
FCriticalSection FNavThreadManager::PathThreadDeletionMutex;
FCriticalSection FNavThreadManager::GraphThreadDeletionMutex;

void FNavThreadManager::AddComponent(UNavComponentAS* Component)
{
	{
		FScopeLock GraphLock(&GraphMutex);
		if (GraphComponents.Num() == 0 && !GraphThread)
		{
			FScopeLock GraphDeletionLock(&GraphThreadDeletionMutex);
			GraphGenerator = new FGraphGeneratorRunnable(Component->GraphGenerationInterval);
			GraphThread = FRunnableThread::Create(GraphGenerator, TEXT("GraphGeneratorThread"));
		}
		GraphComponents.AddUnique(Component);
	}
	{
		FScopeLock PathLock(&PathMutex);
		if (PathComponents.Num() == 0 && !PathThread)
		{
			FScopeLock PathDeletionLock(&PathThreadDeletionMutex);
			PathFinder = new FPathFinderRunnable(Component->FollowUpdateInterval);
			PathThread = FRunnableThread::Create(PathFinder, TEXT("PathFinderThread"));
		}
		PathComponents.AddUnique(Component);
	}
}

void FNavThreadManager::RemoveComponent(UNavComponentAS* Component)
{
	{
		FScopeLock GraphLock(&GraphMutex);
		GraphComponents.Remove(Component);
		if (GraphComponents.Num() == 0 && GraphThread)
		{
			GraphGenerator->Stop();
			// Launch async cleanup to wait for thread completion
			AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, []()
			{
				FScopeLock GraphDeletionLock(&GraphThreadDeletionMutex);
				if (GraphThread)
				{
					UE_LOG(LogTemp, Warning, TEXT("Waiting for graph thread completion..."));
					GraphThread->WaitForCompletion();
					UE_LOG(LogTemp, Warning, TEXT("Graph thread completed. Cleaning up..."));

					if (GraphComponents.Num() == 0)
					{
						delete GraphThread;
						delete GraphGenerator;
						GraphThread = nullptr;
						GraphGenerator = nullptr;
					} else
					{
						GraphGenerator->Restart();
					}
				}
			});
		}
	}
	{
		FScopeLock PathLock(&PathMutex);
		PathComponents.Remove(Component);
		if (PathComponents.Num() == 0 && PathThread)
		{
			PathFinder->Stop();
			AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, []()
			{
				FScopeLock PathDeletionLock(&PathThreadDeletionMutex);
				if (PathThread)
				{
					UE_LOG(LogTemp, Warning, TEXT("Waiting for path thread completion..."));
					PathThread->WaitForCompletion();
					UE_LOG(LogTemp, Warning, TEXT("Path thread completed. Cleaning up..."));

					if (PathComponents.Num() == 0)
					{
						delete PathThread;
						delete PathFinder;
						PathThread = nullptr;
						PathFinder = nullptr;
					} else
					{
						PathFinder->Restart();
					}
				}
			});
		}
	}
}