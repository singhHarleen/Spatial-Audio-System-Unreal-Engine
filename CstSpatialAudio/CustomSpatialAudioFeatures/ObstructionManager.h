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
	static const int32 HitChecksCount;
	AObstructionManager();
	void RegisterEmitter(ACustomEmitter* CustomEmitter);
	void CheckObstruction(ACustomEmitter* Emitter);
protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	float TargetLowPassFrequency;
	UPROPERTY()
	float InterpolationAlpha;
	UPROPERTY()
	float PrevTargetFrequency;
	UPROPERTY()
	float TransitionTime;

	UPROPERTY()
	float LineTraceOffset = 100.0f;
	
	int32 ObstructionCheckFrequency;
	float DebugSphereRadius;
	bool WasOutsideFalloffDistance = true;
	bool JustEnteredFalloffDistance = false;
	
	UPROPERTY()
	float ElapsedTime = 0.0f;
private:
	UPROPERTY()
	TArray<ACustomEmitter*> Emitters;
	int32 CurrentEmitterIndex;
	int32 CurrenTick;
	static constexpr  float FiveHitFilter = 2000.0f;
	static constexpr float FourHitFilter = 4000.0f;
	static constexpr float ThreeHitFilter = 8000.0f;
	static constexpr  float TwoHitFilter = 12000.0f;
	static constexpr float NoFiltering = 20000.0f;

	void UpdateFrequency(int HitCount);
	

public:
	virtual void Tick(float DeltaTime) override;
	static bool DrawObstructionDebug;
	static void ToggleDebugFlag();
};
