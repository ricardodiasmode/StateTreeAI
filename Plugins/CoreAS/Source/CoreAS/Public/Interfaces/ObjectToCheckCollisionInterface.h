// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CollisionCheckerInterface.h"
#include "ObjectToCheckCollisionInterface.generated.h"

UINTERFACE(Blueprintable)
class COREAS_API UObjectToCheckCollisionInterface : public UInterface
{
	GENERATED_BODY()
};

class IObjectToCheckCollisionInterface
{    
	GENERATED_BODY()

public:
	virtual void AddActivator(ICollisionCheckerInterface* Activator) = 0;	
	virtual void RemoveActivator(ICollisionCheckerInterface* Activator) = 0;	
};
