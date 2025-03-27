#pragma once

#include "CoreMinimal.h"
#include "HAL/Runnable.h"
#include "HAL/RunnableThread.h"

class UNavComponentAS;

// Runnable class for periodic graph generation
class FGraphGeneratorRunnable : public FRunnable
{
public:
	FGraphGeneratorRunnable(float InInterval)
		: Interval(InInterval), bRunning(true) {}

	// Initialize the thread
	virtual bool Init() override
	{
		return true;
	}

	// Main thread loop
	virtual uint32 Run() override;

	void Restart()
	{
		bRunning = true;
		Run();
	}

	// Stop the thread gracefully
	virtual void Stop() override
	{
		bRunning = false;
	}

private:
	float Interval;             // Time between calls (in seconds)
	bool bRunning;                 // Flag to stop the thread
};
