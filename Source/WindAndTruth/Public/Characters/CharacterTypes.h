#pragma once

//Creating Enum for PlayerStates
UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	ECS_Unequipped UMETA(DisplayName = "Unequipped"),
	ECS_EquippedOneHandedWeapon UMETA(DisplayName = "Equipped One-Handed"),
	ECS_EquippedTwoHandedWeapon UMETA(DisplayName = "Equipped Two-Handed"),
};