#pragma once
#include "Runnables/GraphGeneratorRunnable.h"
#include "Runnables/PathFinderRunnable.h"
#include "HAL/Runnable.h"
#include "HAL/RunnableThread.h"
#include "NavThreadManager.generated.h"

USTRUCT()
struct NAVIGATIONAS_API FNavThreadManager
{
	GENERATED_BODY()
	
	public:
	static void AddComponent(UNavComponentAS* Component);
	static void RemoveComponent(UNavComponentAS* Component);

private:
	// Shared threads and runnables
	static FRunnableThread* GraphThread;
	static FRunnableThread* PathThread;
	static FGraphGeneratorRunnable* GraphGenerator;
	static FPathFinderRunnable* PathFinder;

public:
	// Component lists with thread safety
	static FCriticalSection GraphMutex;
	static TArray<TWeakObjectPtr<UNavComponentAS>> GraphComponents;
	static TArray<TWeakObjectPtr<UNavComponentAS>> PathComponents;
	static FCriticalSection PathMutex;
	
	static FCriticalSection TargetMutex;
	static FCriticalSection GraphThreadDeletionMutex;
	static FCriticalSection PathThreadDeletionMutex;
};
