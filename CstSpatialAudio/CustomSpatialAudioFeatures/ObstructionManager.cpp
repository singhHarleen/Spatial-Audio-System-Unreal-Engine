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

#include "ObstructionManager.h"
#include "VectorTypes.h"
#include "../../../../UE_5.1/Engine/Source/Runtime/Core/Public/Containers/Array.h"
#include "../../../../UE_5.1/Engine/Source/Runtime/Core/Public/Containers/Map.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"

// Please note: A tick check would be removed to be replaced by a Timer so the check can be framerate-independent

bool AObstructionManager::DrawObstructionDebug = false;
const int32 AObstructionManager::HitChecksCount = 5;

AObstructionManager::AObstructionManager() :
ObstructionCheckFrequency(0.01f),
DebugSphereRadius(50.0f)
{
	CurrenTick = 0.0f;
}

void AObstructionManager::BeginPlay()	
{
	Super::BeginPlay();

	PlayerCameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);

	GetWorld()->GetTimerManager().SetTimer(ObstructionCheckTimerHandle, this, &AObstructionManager::ObstructionCheck,
											ObstructionCheckFrequency, true);

	AudioTraceChannel = ECollisionChannel::ECC_GameTraceChannel1;
}

void AObstructionManager::RegisterEmitter(ACustomEmitter* CustomEmitter)
{
	Emitters.Add(CustomEmitter);
}

void AObstructionManager::UnregisterEmitter(ACustomEmitter* CustomEmitter)
{
	Emitters.Remove(CustomEmitter);

	if (CurrentEmitterIndex >= Emitters.Num())
	{
		CurrentEmitterIndex = 0;
	}
}

void AObstructionManager::CheckObstruction(ACustomEmitter* CustomEmitter)
{

	if (!PlayerCameraManager || !CustomEmitter)
	{
		return; // Early return because no point checking if this is the case
	}
	
	const float DistanceCameraEmitter = UE::Geometry::Distance(PlayerCameraManager->GetCameraLocation(),
										CustomEmitter->AudioComponent->GetComponentLocation());

	const bool IsListenerInFallOffRange = CustomEmitter->AudioComponent->AttenuationOverrides.FalloffDistance >=DistanceCameraEmitter;

	JustEnteredFalloffDistance = WasOutsideFalloffDistance && IsListenerInFallOffRange;
	
	if (IsListenerInFallOffRange)
	{
		
			TArray<TPair<FVector, FVector>> Vectors = GenerateLineTraceVectors(CustomEmitter, PlayerCameraManager);
			TArray<FHitResult> HitResults[5];
		
			int HitCount = PerformLineTraces(Vectors, HitResults);
		
			UpdateFrequency(HitCount);
			UpdateLowPassFilterFrequency(CustomEmitter);
	

// Wrap this in a non-shipping build macro
			DrawDebugInfo(HitResults, Vectors, HitCount);
	}
	// Wrap this in a non-shipping build macro
	WasOutsideFalloffDistance = !IsListenerInFallOffRange;
}

void AObstructionManager::ObstructionCheck()
{
	if (Emitters.Num() > 0)
	{
		CheckObstruction(Emitters[CurrentEmitterIndex]);
		CurrentEmitterIndex = (CurrentEmitterIndex + 1) % Emitters.Num();
	}
}

void AObstructionManager::UpdateFrequency(int HitCount)
{
	switch (HitCount)
	{
	case 5:
		TargetLowPassFrequency = FiveHitFilter;
		break;
	case 4:
		TargetLowPassFrequency = FourHitFilter;
		break;
	case 3:
		TargetLowPassFrequency = ThreeHitFilter;
		break;
	case 2:
		TargetLowPassFrequency = TwoHitFilter;
		break;
	default:
		TargetLowPassFrequency = NoFiltering;
		break;
	}
}

TArray<TPair<FVector, FVector>> AObstructionManager::GenerateLineTraceVectors(ACustomEmitter* Emitter,
	APlayerCameraManager* PlayerCameraManager)
{
	FVector Start = CustomEmitter->GetActorLocation();
	FVector End = PlayerCameraManager->GetCameraLocation();
	FVector CameraRightVector = PlayerCameraManager->GetActorRightVector();
	FVector CameraUpVector = PlayerCameraManager->GetActorUpVector();
	float OffsetFromCamera = LineTraceOffset;

	AllVectors.Add({ Start - CameraRightVector * OffsetFromCamera, End - CameraRightVector * OffsetFromCamera });
	AllVectors.Add({ Start, End });
	AllVectors.Add({ Start + CameraRightVector * OffsetFromCamera, End + CameraRightVector * OffsetFromCamera });
	AllVectors.Add({ Start + CameraUpVector * OffsetFromCamera, End + CameraUpVector * OffsetFromCamera });
	AllVectors.Add({ Start - CameraUpVector * OffsetFromCamera, End - CameraUpVector * OffsetFromCamera });
}

#if !UE_BUILD_SHIPPING
int AObstructionManager::PerformLineTraces(TArray<TPair << Fector, FVector>, Vectors, TArray<FHitResult> HitResults[])
{
	int HitCount = 0;
	for (int i = 0; i < HitChecksCount; ++i)
	{
		if (GetWorld()->LineTraceMultiByChannel(AllHitResults[i],
			AllVectors[i].Key, AllVectors[i].Value, AudioTraceChannel))
		{
			++HitCount;
		}
	}
	return HitCount;
}

void AObstructionManager::UpdateLowPassFilterFrequency(ACustomEmitter* Emitter)
{
	const float LerpRatio = FMath::Clamp(ElapsedTime / TransitionTime, 0.0f, 1.0f);
	if (JustEnteredFalloffDistance)
	{
		Emitter->AudioComponent->SetLowPassFilterFrequency(TargetLowPassFrequency);
	}
	else
	{
		PrevTargetFrequency = FMath::Lerp(PrevTargetFrequency, TargetLowPassFrequency, LerpRatio);
		Emitter->AudioComponent->SetLowPassFilterFrequency(PrevTargetFrequency);
	}
	if (LerpRatio >= 1.0f)
	{
		ElapsedTime = 0.0f;
	}
}

void AObstructionManager::DrawDebugInfo(const TArray<FHitResult> HitResults,
                                        const TArray<TPair<FVector, FVector>>& Vectors, int HitCount)
{
}
#endif 
void AObstructionManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AObstructionManager::ToggleDebugFlag()
{
	DrawObstructionDebug = !DrawObstructionDebug;
}