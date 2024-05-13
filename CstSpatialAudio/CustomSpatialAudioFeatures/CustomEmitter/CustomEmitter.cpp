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

#include "CustomEmitter.h"
#include "CoreMinimal.h"
#include "EngineUtils.h"
#include "GameFramework/Actor.h"
#include "Components/AudioComponent.h"
#include "CstSpatialAudio/CustomSpatialAudioFeatures/ObstructionManager.h"

ACustomEmitter::ACustomEmitter() :
ObstructionCheckInterval(1.0f),
TargetLowPassFrequency(20000.0f),
InterpolationAlpha(0.0f),
PrevTargetFrequency(0.0f),
TransitionTime(1.0f)
{
    PrimaryActorTick.bCanEverTick = true;
    AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
}

ACustomEmitter::~ACustomEmitter()
{
    if (ObstructionManager)
    {
        ObstructionManager->UnregisterEmitter(this);
    }
}

void ACustomEmitter::BeginPlay()
{
    Super::BeginPlay();
    AudioComponent->SetLowPassFilterEnabled(true);
    ObstructionManager = nullptr;
    
    for (TActorIterator<AObstructionManager> It(GetWorld()); It; ++It)
    {
        ObstructionManager = *It;
        break;
    }
    if(ObstructionManager)
    {
        ObstructionManager->RegisterEmitter(this);
    }
    AudioComponent->SetLowPassFilterEnabled(true);
}

void ACustomEmitter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    ElapsedTime += DeltaTime;
    const float LerpRatio = FMath::Clamp(ElapsedTime / TransitionTime, 0.0f, 1.0f);
    PrevTargetFrequency = FMath::Lerp(PrevTargetFrequency, TargetLowPassFrequency, LerpRatio);
    AudioComponent->SetLowPassFilterFrequency(PrevTargetFrequency);
    if (LerpRatio >= 1.0f)
        ElapsedTime = 0.0f;
}