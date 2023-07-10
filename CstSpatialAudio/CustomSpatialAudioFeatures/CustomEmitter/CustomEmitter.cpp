#include "CustomEmitter.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"

ACustomEmitter::ACustomEmitter() :
ObstructionCheckInterval(1.0f),
TargetLowPassFrequency(20000.0f),
InterpolationAlpha(0.0f),
prevTargetFrequency(0.0f),
transitionTime(1.0f)
{
    PrimaryActorTick.bCanEverTick = true;
    AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
}
void ACustomEmitter::BeginPlay()
{
    Super::BeginPlay();
    GetWorld()->GetTimerManager().SetTimer(CheckObstructionTimer,this, &ACustomEmitter::CheckObstruction, 0.25f, true);
    AudioComponent->SetLowPassFilterEnabled(true);
}

void ACustomEmitter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    elapsedTime += DeltaTime;
    const float LerpRatio = FMath::Clamp(elapsedTime / transitionTime, 0.0f, 1.0f);
    prevTargetFrequency = FMath::Lerp(prevTargetFrequency, TargetLowPassFrequency, LerpRatio);
    AudioComponent->SetLowPassFilterFrequency(prevTargetFrequency);
    if (LerpRatio >= 1.0f)
    {
        elapsedTime = 0.0f;
    }
}

void ACustomEmitter::CheckObstruction()
{
    APlayerCameraManager* PCM = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
    if(CameraCacheLocation != PCM->GetCameraLocation())
    {
        CameraCacheLocation = PCM->GetCameraLocation();
        TArray<FHitResult> HitResults1, HitResults2, HitResults3;

        FVector Start = GetActorLocation();
        FVector End = PCM->GetCameraLocation(); 
        float Distance = FVector::Dist(Start, End);

        FVector CameraRightVector = PCM->GetActorRightVector();
        float YourDesiredOffset = 100.0f;

        FVector Offset = CameraRightVector * YourDesiredOffset;

        FVector Start1 = Start - Offset;
        FVector End1 = End - Offset;

        FVector Start2 = Start;
        FVector End2 = End;

        FVector Start3 = Start + Offset;
        FVector End3 = End + Offset;

        ECollisionChannel AudioTraceChannel = ECollisionChannel::ECC_GameTraceChannel1;

        bool bHit1 = GetWorld()->LineTraceMultiByChannel(HitResults1, Start1, End1, AudioTraceChannel);
        bool bHit2 = GetWorld()->LineTraceMultiByChannel(HitResults2, Start2, End2, AudioTraceChannel);
        bool bHit3 = GetWorld()->LineTraceMultiByChannel(HitResults3, Start3, End3, AudioTraceChannel);
        
        // Array to store all hit results
        TArray<TArray<FHitResult>> AllHitResults = {HitResults1, HitResults2, HitResults3};
        // Array to store all start and end vectors
        TArray<TPair<FVector, FVector>> AllVectors = {{Start1, End1}, {Start2, End2}, {Start3, End3}};

        int index = 0;
        const FColor DebugLineColor = FColor::Red;

        bool isOneLineHit = false;
        bool isTwoLineHit = false;
        bool isThreeLineHit = false;

        if (bHit1 && bHit2 || bHit2 && bHit3 || bHit1 && bHit3)
            isTwoLineHit = true;

        if(bHit1 && bHit2 && bHit3)
            isThreeLineHit = true;

        if (bHit1 || bHit2 || bHit3)
            isOneLineHit = true;
        
        if (isThreeLineHit)
        {
            TargetLowPassFrequency = 2000.0f;
        }
        else if(isTwoLineHit)
        {
            TargetLowPassFrequency = 4000.0f;
        }
        else
        {
            TargetLowPassFrequency = 20000.0f;
        }
        
        if (isTwoLineHit && DrawSphereAndLines)
        {
            for (auto& HitResult : AllHitResults)
            {
                if (HitResult.Num() > 0)
                {
                    FHitResult& Hit = HitResult[0];
                    float SphereOffsetFactor = 0.2f;
                    FVector SphereLocation = FMath::Lerp(Hit.ImpactPoint, AllVectors[index].Value, SphereOffsetFactor);

                    DrawDebugSphere(GetWorld(), SphereLocation, 50.0f, 4, FColor::Purple, false,
                             0.01f, 0, 1);
                    
                    // DrawDebugLine(GetWorld(), AllVectors[index].Key, AllVectors[index].Value, DebugLineColor, false,
                    //             0.1f, 0, 0.5f);
                }
                ++index;
            }
            GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ACustomEmitter::CheckObstruction);
        }
    }
}
