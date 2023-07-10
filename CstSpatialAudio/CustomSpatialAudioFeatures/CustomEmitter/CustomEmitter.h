// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EmitterSpawn.h"
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
	float ObstructionCheckInterval; 

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
	float transitionTime;
	UPROPERTY()
	float elapsedTime = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool DrawSphereAndLines = false;
	FTimerHandle CheckObstructionTimer;
	FVector CameraCacheLocation;
};
