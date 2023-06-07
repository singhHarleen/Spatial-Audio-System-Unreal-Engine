// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CustomEmitter.generated.h"

UCLASS()
class CSTSPATIALAUDIO_API ACustomEmitter : public AActor
{
	GENERATED_BODY()
public:	
	ACustomEmitter();
	
	virtual void Tick(float DeltaTime) override;
	
	void CheckObstruction();
	
	float ObstructionCheckInterval = 0.1f; 

protected:

	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere)
	class UAudioComponent* AudioComponent;

	UPROPERTY()
	float TargetLowPassFrequency;

	UPROPERTY()
	float InterpolationAlpha;

	UPROPERTY()
	float prevTargetFrequency;

	UPROPERTY()
	float transitionTime = 5.0f;

	UPROPERTY()
	float elapsedTime = 0.0f;
};
