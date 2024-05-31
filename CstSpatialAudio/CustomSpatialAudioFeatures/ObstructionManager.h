/*
* Copyright (c) 2024 Harleen Singh
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#pragma once

#include "CoreMinimal.h"
#include "../../../../UE_5.1/Engine/Source/Runtime/Core/Public/Containers/Array.h"
#include "../../../../UE_5.1/Engine/Source/Runtime/Core/Public/Containers/Map.h"
#include "../../../../UE_5.1/Engine/Source/Runtime/Engine/Classes/Engine/HitResult.h"
#include "CustomEmitter/ACustomEmitter.h"
#include "GameFramework/Actor.h"
#include "ObstructionManager.generated.h"

enum ECollisionChannel : int;
class APlayerCameraManager;

namespace ispc
{
	struct FVector;
}

UCLASS()
class CSTSPATIALAUDIO_API AObstructionManager : public AActor
{
	GENERATED_BODY()

public:
	static const int32 HitChecksCount;
	AObstructionManager();
	void RegisterEmitter(ACustomEmitter* CustomEmitter);
	void UnregisterEmitter(ACustomEmitter* CustomEmitter);
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
	
	float ObstructionCheckFrequency;
	float DebugSphereRadius;
	bool WasOutsideFalloffDistance = true;
	bool JustEnteredFalloffDistance = false;
	
	UPROPERTY()
	float ElapsedTime = 0.0f;

	void ObstructionCheck();
private:
	UPROPERTY()
	TArray<ACustomEmitter*> Emitters;

	ECollisionChannel AudioTraceChannel;
	
	int32 CurrentEmitterIndex;
	
	int32 CurrenTick;
	
	static constexpr  float FiveHitFilter = 2000.0f;
	static constexpr float FourHitFilter = 4000.0f;
	static constexpr float ThreeHitFilter = 8000.0f;
	static constexpr  float TwoHitFilter = 12000.0f;
	static constexpr float NoFiltering = 20000.0f;

	void UpdateFrequency(int HitCount);

	TArray<TPair<FVector, FVector>> GenerateLineTraceVectors(ACustomEmitter* Emitter, APlayerCameraManager* PlayerCameraManager);
	
	APlayerCameraManager* PlayerCameraManager;

	
	FTimerHandle ObstructionCheckTimerHandle;

	int PerformLineTraces(TArray<TPair<<Fector, FVector>>& Vectors, TArray<FHitResult> HitResults[]);

	void UpdateLowPassFilterFrequency(ACustomEmitter* Emitter);

#if !UE_BUILD_SHIPPING
	void DrawDebugInfo(const TArray<FHitResult> HitResults, const TArray<TPair<FVector, FVector>>&Vectors, int HitCount);
#endif
	
public:
	virtual void Tick(float DeltaTime) override;
	
	static bool DrawObstructionDebug;

	static void ToggleDebugFlag();
};
