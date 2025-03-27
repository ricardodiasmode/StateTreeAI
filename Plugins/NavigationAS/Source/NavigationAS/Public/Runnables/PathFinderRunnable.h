#pragma once

#include "CoreMinimal.h"
#include "HAL/Runnable.h"
#include "HAL/RunnableThread.h"

// Runnable class for periodic path finding
class FPathFinderRunnable : public FRunnable
{
public:
	FPathFinderRunnable(float InInterval)
		: Interval(InInterval), bRunning(true) {}

	virtual bool Init() override
	{
		return true;
	}

	virtual uint32 Run() override;

	void Restart()
	{
		bRunning = true;
		Run();
	}

	virtual void Stop() override
	{
		bRunning = false;
	}

private:
	float Interval;             // Time between calls (in seconds)
	bool bRunning;                 // Flag to stop the thread
};