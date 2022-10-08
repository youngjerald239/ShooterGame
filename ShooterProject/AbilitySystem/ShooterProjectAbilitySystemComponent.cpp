// Copyright Epic Games, Inc. All Rights Reserved.

#include "ShooterProjectAbilitySystemComponent.h"
#include "ShooterProject/Animation/ShooterProjectAnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "ShooterProject/AbilitySystem/ShooterProjectAbilityAttributeSet.h"

void UShooterProjectAbilitySystemComponent::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	Super::InitAbilityActorInfo(InOwnerActor, InAvatarActor);

	if (AbilityActorInfo)
	{
		if (AbilityActorInfo->AnimInstance == nullptr)
		{
			AbilityActorInfo->AnimInstance = AbilityActorInfo->GetAnimInstance();
		}

		if (UShooterProjectAnimInstance* TopAnimInststance = Cast<UShooterProjectAnimInstance>(AbilityActorInfo->AnimInstance))
		{
			TopAnimInststance->InitializeWithAbilitySystem(this);
		}

		if (AbilityActorInfo->SkeletalMeshComponent.IsValid())
		{
			const USkeletalMeshComponent* SkeletalMeshComponent = AbilityActorInfo->SkeletalMeshComponent.Get();

			const TArray<UAnimInstance*>& AnimLayers = SkeletalMeshComponent->GetLinkedAnimInstances();
			for (UAnimInstance* AnimLayer : AnimLayers)
			{
				if (UShooterProjectAnimInstance* ShooterProjectAnimLayer = Cast<UShooterProjectAnimInstance>(AnimLayer))
				{
					ShooterProjectAnimLayer->InitializeWithAbilitySystem(this);
				}
			}
		}

		if (UGameInstance* GameInstance = InOwnerActor->GetGameInstance())
		{
			// Sign up for possess/unpossess events so that we can update the cached AbilityActorInfo accordingly
			GameInstance->GetOnPawnControllerChanged().AddDynamic(this, &UShooterProjectAbilitySystemComponent::OnPawnControllerChanged);
		}
	}

	GrantDefaultAbilitiesAndAttributes();
}

void UShooterProjectAbilitySystemComponent::BeginDestroy()
{
	if (AbilityActorInfo && AbilityActorInfo->OwnerActor.IsValid())
	{
		if (UGameInstance* GameInstance = AbilityActorInfo->OwnerActor->GetGameInstance())
		{
			GameInstance->GetOnPawnControllerChanged().RemoveAll(this);
		}
	}

	Super::BeginDestroy();
}

FGameplayAbilitySpecHandle UShooterProjectAbilitySystemComponent::GrantAbilityOfType(TSubclassOf<UGameplayAbility> AbilityType, bool bRemoveAfterActivation)
{
	FGameplayAbilitySpecHandle AbilityHandle;
	if (AbilityType)
	{
		FGameplayAbilitySpec AbilitySpec(AbilityType);
		AbilitySpec.RemoveAfterActivation = bRemoveAfterActivation;

		AbilityHandle = GiveAbility(AbilitySpec);
	}
	return AbilityHandle;
}

void UShooterProjectAbilitySystemComponent::GrantDefaultAbilitiesAndAttributes()
{
	// Reset/Remove abilities if we had already added them
	{
		for (UAttributeSet* AttribSetInstance : AddedAttributes)
		{
			GetSpawnedAttributes_Mutable().Remove(AttribSetInstance);
		}

		for (FGameplayAbilitySpecHandle AbilityHandle : DefaultAbilityHandles)
		{
			SetRemoveAbilityOnEnd(AbilityHandle);
		}

		AddedAttributes.Empty(DefaultAttributes.Num());
		DefaultAbilityHandles.Empty(DefaultAbilities.Num());
	}

	// Default abilities
	{
		DefaultAbilityHandles.Reserve(DefaultAbilities.Num());
		for (const TSubclassOf<UGameplayAbility>& Ability : DefaultAbilities)
		{
			if (*Ability)
			{
				DefaultAbilityHandles.Add(GiveAbility(FGameplayAbilitySpec(Ability)));
			}
		}
	}

	// Default attributes
	{
		for (const FShooterProjectAttributeApplication& Attributes : DefaultAttributes)
		{
			if (Attributes.AttributeSetType)
			{
				UAttributeSet* NewAttribSet = NewObject<UAttributeSet>(this, Attributes.AttributeSetType);
				if (Attributes.InitializationData)
				{
					NewAttribSet->InitFromMetaDataTable(Attributes.InitializationData);
				}
				AddedAttributes.Add(NewAttribSet);
				AddAttributeSetSubobject(NewAttribSet);
			}
		}
	}
}

void UShooterProjectAbilitySystemComponent::OnPawnControllerChanged(APawn* Pawn, AController* NewController)
{
	if (AbilityActorInfo && AbilityActorInfo->OwnerActor == Pawn && AbilityActorInfo->PlayerController != NewController)
	{
		// Reinit the cached ability actor info (specifically the player controller)
		AbilityActorInfo->InitFromActor(AbilityActorInfo->OwnerActor.Get(), AbilityActorInfo->AvatarActor.Get(), this);
	}
}
