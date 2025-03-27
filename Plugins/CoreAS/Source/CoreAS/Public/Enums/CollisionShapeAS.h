#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class ECollisionShapeAS : uint8
{
	NONE,
	BOX,
	CAPSULE,
	SPHERE,
	TRACE,
};
