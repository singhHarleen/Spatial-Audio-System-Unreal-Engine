// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CustomEmitter/CustomEmitter.h"
#include "GameFramework/Actor.h"
#include "ObstructionManager.generated.h"

UCLASS()
class CSTSPATIALAUDIO_API AObstructionManager : public AActor
{
	GENERATED_BODY()

public:
	AObstructionManager();
	void RegisterEmitter(ACustomEmitter* CustomEmitter);
	void CheckObstruction(ACustomEmitter* Emitter);

protected:
	virtual void BeginPlay() override;
	FVector CameraCacheLocation;

	UPROPERTY()
	float TargetLowPassFrequency;
	UPROPERTY()
	float InterpolationAlpha;
	UPROPERTY()
	float PrevTargetFrequency;
	UPROPERTY()
	float TransitionTime;
	bool DrawSphereAndLines = false;
	int32 ObstructionCheckFrequency;

	ACustomEmitter* CurrentlyProcessedEmitter = nullptr;
private:
	UPROPERTY()
	TArray<ACustomEmitter*> Emitters;
	int32 CurrentEmitterIndex;
	int32 CurrenTick;
public:
	virtual void Tick(float DeltaTime) override;
};
