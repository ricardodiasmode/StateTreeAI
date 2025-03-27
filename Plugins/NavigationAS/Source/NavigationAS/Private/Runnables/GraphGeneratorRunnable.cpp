#pragma once

#include "Runnables/GraphGeneratorRunnable.h"
#include "Components/NavComponentAS.h"
#include "NavigationAS/NavThreadManager.h"

uint32 FGraphGeneratorRunnable::Run()
{
	while (bRunning)
	{
		TArray<TWeakObjectPtr<UNavComponentAS>> Components;
		{
			FScopeLock Lock(&FNavThreadManager::GraphMutex);
			Components = FNavThreadManager::GraphComponents;
		}
		
		for (TWeakObjectPtr<UNavComponentAS> Component : Components)
		{
			if (!Component.IsValid(true, true) ||
				Component.IsStale(true, true))
				continue;
			
			if (UNavComponentAS* DerefComp = Component.Get())
				DerefComp->GenerateGraph();
		}
		FPlatformProcess::Sleep(Interval);
	}
	return 0;
}
