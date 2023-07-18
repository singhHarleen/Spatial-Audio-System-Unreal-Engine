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
	float ObstructionCheckInterval;
	UPROPERTY()
	class UAudioComponent* AudioComponent;

protected:
	virtual void BeginPlay() override;

	
	//UPROPERTY(VisibleAnywhere)
	
	UPROPERTY()
	float TargetLowPassFrequency;
	UPROPERTY()
	float InterpolationAlpha;
	UPROPERTY()
	float PrevTargetFrequency;
	UPROPERTY()
	float TransitionTime;
	UPROPERTY()
	float ElapsedTime = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool DrawSphereAndLines = false;
	FTimerHandle CheckObstructionTimer;
	FVector CameraCacheLocation;
};
