// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TickableInterface.generated.h"

UINTERFACE(Blueprintable)
class COREAS_API UTickableInterface : public UInterface
{
	GENERATED_BODY()
};

class ITickableInterface
{    
	GENERATED_BODY()

public:
	virtual void TickFromManager() = 0;	
};
