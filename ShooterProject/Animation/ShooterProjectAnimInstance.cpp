// Copyright Epic Games, Inc. All Rights Reserved.

#include "ShooterProjectAnimInstance.h"
#include "AbilitySystemComponent.h"

void UShooterProjectAnimInstance::NativeInitializeAnimation()
{
	if (AActor* MyActor = GetOwningActor())
	{
		UAbilitySystemComponent* AbilityComponent = MyActor->FindComponentByClass<UAbilitySystemComponent>();
		if (AbilityComponent)
		{
			InitializeWithAbilitySystem(AbilityComponent);
		}
	}

	Super::NativeInitializeAnimation();
}

void UShooterProjectAnimInstance::InitializeWithAbilitySystem(UAbilitySystemComponent* AbilityComponent)
{
	GameplayTagPropertyMap.Initialize(this, AbilityComponent);
	ReceiveAbilitySystem(AbilityComponent);
}