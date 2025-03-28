#pragma once

#include "Runnables/PathFinderRunnable.h"
#include "Components/NavComponentAS.h"
#include "NavigationAS/NavThreadManager.h"

uint32 FPathFinderRunnable::Run()
{
	while (bRunning)
	{
		TArray<TWeakObjectPtr<UNavComponentAS>> Components;
		{
			FScopeLock Lock(&FNavThreadManager::PathMutex);
			Components = FNavThreadManager::PathComponents;
		}

		for (TWeakObjectPtr<UNavComponentAS> Component : Components)
		{
			if (!Component.IsValid(true, true) ||
				Component.IsStale(true, true))
				continue;
			
			if (UNavComponentAS* DerefComp = Component.Get())
				DerefComp->FindPathToClosestPointOnGraph();
		}
		FPlatformProcess::Sleep(Interval); // e.g., 0.1f seconds
	}
	return 0;
}
