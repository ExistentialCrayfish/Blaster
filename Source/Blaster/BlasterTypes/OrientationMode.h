#pragma once

UENUM(BlueprintType)
enum class EOrientationMode : uint8
{
	EOM_OrientWithMovement UMETA(DisplayName = "Orient With Movement"),
	EOM_LockToYaw UMETA(DisplayName = "Lock rotation to yaw"),

	EOM_MAX UMETA(DisplayName = "DefaultMAX"),
};