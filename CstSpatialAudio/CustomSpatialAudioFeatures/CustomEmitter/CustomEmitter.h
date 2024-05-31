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

#include "../../../../../UE_5.1/Engine/Source/Runtime/Core/Public/CoreMinimal.h";
#include "../../../../../UE_5.1/Engine/Source/Runtime/Engine/Classes/GameFramework/Actor.h"
#include "CustomEmitter.generated.h"

class AObstructionManager;

UCLASS()
class CSTSPATIALAUDIO_API ACustomEmitter : public AActor
{
	GENERATED_BODY()
public:	
	ACustomEmitter();
	virtual ~ACustomEmitter() override;
	
	virtual void Tick(float DeltaTime) override;
	float ObstructionCheckInterval;
	UPROPERTY()
	UAudioComponent* AudioComponent;

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
	
	float ElapsedTime = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	
	bool DrawSphereAndLines = false;
	
	// FTimerHandle CheckObstructionTimer;
	
	FVector CameraCacheLocation;

	UPROPERTY()
	AObstructionManager* ObstructionManager;
};
