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
	FVector EmitterCacheLocatin;

	UPROPERTY()
	float TargetLowPassFrequency;
	UPROPERTY()
	float InterpolationAlpha;
	UPROPERTY()
	float PrevTargetFrequency;
	UPROPERTY()
	float TransitionTime;
	int32 ObstructionCheckFrequency;
	float DebugSphereRadius;
	bool WasOutsideFalloffDistance = true;
	bool JustEnteredFalloffDistance = false;
	
	ACustomEmitter* CurrentlyProcessedEmitter = nullptr;
	UPROPERTY()
	float ElapsedTime = 0.0f;
private:
	UPROPERTY()
	TArray<ACustomEmitter*> Emitters;
	int32 CurrentEmitterIndex;
	int32 CurrenTick;
	static constexpr  float MaxFiltering = 2000.0f;
	static constexpr float MidFiltering = 4000.0f;
	static constexpr float NoFiltering = 20000.0f;
	

public:
	virtual void Tick(float DeltaTime) override;
	static bool DrawObstructionDebug;
	void ToggleDebugFlag();
};
