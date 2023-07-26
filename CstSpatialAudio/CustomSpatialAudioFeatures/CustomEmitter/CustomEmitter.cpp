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
void ACustomEmitter::BeginPlay()
{
    Super::BeginPlay();
    AudioComponent->SetLowPassFilterEnabled(true);

    AObstructionManager* ObstructionManager = nullptr;
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

