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

	GetWorld()->GetTimerManager().SetTimer(ObstructionCheckTimerHandle, this, &AObstructionManager::ObstructionCheck,
											ObstructionCheckFrequency, true);
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
	APlayerCameraManager* PlayerCameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);

	const float DistanceCameraEmitter = UE::Geometry::Distance(PlayerCameraManager->GetCameraLocation(),
										CustomEmitter->AudioComponent->GetComponentLocation());

	const bool IsListenerInFallOffRange = CustomEmitter->AudioComponent->AttenuationOverrides.FalloffDistance >= DistanceCameraEmitter;
	JustEnteredFalloffDistance = WasOutsideFalloffDistance && IsListenerInFallOffRange;
	
	if (IsListenerInFallOffRange)
	{
			TArray<FHitResult> HitResults1, HitResults2, HitResults3, HitResults4, HitResults5;

	        FVector Start = CustomEmitter->GetActorLocation();
	        FVector End = PlayerCameraManager->GetCameraLocation();
	        FVector CameraRightVector = PlayerCameraManager->GetActorRightVector();
			FVector CameraUpVector = PlayerCameraManager->GetActorUpVector();
	        float OffsetFromCamera = LineTraceOffset;

	        FVector OffsetRight = CameraRightVector * OffsetFromCamera;
			FVector OffsetUp = CameraUpVector * OffsetFromCamera;

	        FVector Start1 = Start - OffsetRight;
	        FVector End1 = End - OffsetRight;

	        FVector Start2 = Start;
	        FVector End2 = End;

	        FVector Start3 = Start + OffsetRight;
	        FVector End3 = End + OffsetRight;

			FVector Start4 = Start + OffsetUp;
			FVector End4 = End + OffsetUp;
			FVector Start5 = Start - OffsetUp;
			FVector End5 = End - OffsetUp;

	        ECollisionChannel AudioTraceChannel = ECollisionChannel::ECC_GameTraceChannel1;
		
	        // Array to store all hit results
			TArray<FHitResult> AllHitResults[5];
	        // Array to store all start and end vectors
	        TArray<TPair<FVector, FVector>> AllVectors = {{Start1, End1}, {Start2, End2},
		        											{Start3, End3}, {Start4, End4},
															{Start5, End5}};
		
			bool boolHits[HitChecksCount];
			
			boolHits[0] = GetWorld()->LineTraceMultiByChannel(AllHitResults[0], Start1, End1, AudioTraceChannel);
			boolHits[1] = GetWorld()->LineTraceMultiByChannel(AllHitResults[1], Start2, End2, AudioTraceChannel);
			boolHits[2] = GetWorld()->LineTraceMultiByChannel(AllHitResults[2], Start3, End3, AudioTraceChannel);
			boolHits[3] = GetWorld()->LineTraceMultiByChannel(AllHitResults[3], Start4, End4, AudioTraceChannel);
			boolHits[4] = GetWorld()->LineTraceMultiByChannel(AllHitResults[4], Start5, End5, AudioTraceChannel);

			int HitCount = 0;

			for (int i = 0; i < HitChecksCount; i++)
			{
				if (boolHits[i])
				{
					HitCount++;	
				}
			}

			UpdateFrequency(HitCount);
		
			const float LerpRatio = FMath::Clamp(ElapsedTime / TransitionTime, 0.0f, 1.0f);
			if (JustEnteredFalloffDistance)
			{
				CustomEmitter->AudioComponent->SetLowPassFilterFrequency(TargetLowPassFrequency);
			}
			else
			{
				PrevTargetFrequency = FMath::Lerp(PrevTargetFrequency, TargetLowPassFrequency, LerpRatio);
				CustomEmitter->AudioComponent->SetLowPassFilterFrequency(PrevTargetFrequency);
			}
			if (LerpRatio >= 1.0f)
				ElapsedTime = 0.0f;
		
	        if (DrawObstructionDebug && HitCount >= 2)
	        {
		        int Index = 0;
		        for (auto& HitResult : AllHitResults)
	            {
	                if (HitResult.Num() > 0)
	                {
	                    FHitResult& Hit = HitResult[0];
	                    float SphereOffsetFactor = 0.2f;
	                    FVector SphereLocation = FMath::Lerp(Hit.ImpactPoint, AllVectors[Index].Value, SphereOffsetFactor);

	                    DrawDebugSphere(GetWorld(), Hit.ImpactPoint, DebugSphereRadius, 4, FColor::Purple, false,
	                             0.1f, 0, 5);
	                    
	                    DrawDebugLine(GetWorld(), AllVectors[Index].Key, AllVectors[Index].Value, FColor::Red, false,
	                                0.1f, 0, 0.5f);
	                }
	                ++Index; 
	            }
	        }
	}
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

void AObstructionManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AObstructionManager::ToggleDebugFlag()
{
	DrawObstructionDebug = !DrawObstructionDebug;
}