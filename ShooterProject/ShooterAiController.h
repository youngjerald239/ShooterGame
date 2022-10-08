// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "ShooterAiController.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERPROJECT_API AShooterAiController : public AAIController
{
	GENERATED_BODY()
public:
	virtual void Tick(float DeltaSeconds) override;
	bool IsDead() const;

protected:
	virtual void BeginPlay() override;
	

private:
	UPROPERTY(EditAnywhere)
	// float AcceptanceRadius = 200;
	class UBehaviorTree* AIBehavior;
	
};
