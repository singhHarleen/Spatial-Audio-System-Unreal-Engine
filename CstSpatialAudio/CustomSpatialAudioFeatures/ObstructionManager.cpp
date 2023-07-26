#include "ObstructionManager.h"

#include "VectorTypes.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"

AObstructionManager::AObstructionManager() :
ObstructionCheckFrequency(15)
{
	PrimaryActorTick.bCanEverTick = true;
}

void AObstructionManager::RegisterEmitter(ACustomEmitter* CustomEmitter)
{
	// This seems to reallocate the whole array when adding a new item? Maybe we need a better way to handle this 
	Emitters.Add(CustomEmitter);
}

void AObstructionManager::CheckObstruction(ACustomEmitter* CustomEmitter)
{
	APlayerCameraManager* PlayerCameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);

    float DistanceCameraEmitter = UE::Geometry::Distance(PlayerCameraManager->GetCameraLocation(), CustomEmitter->AudioComponent->GetComponentLocation());
    bool IsEmitterInFallOffDistance = CustomEmitter->AudioComponent->AttenuationOverrides.FalloffDistance >= DistanceCameraEmitter;
    
    if(CameraCacheLocation != PlayerCameraManager->GetCameraLocation() && IsEmitterInFallOffDistance)
    {
        CameraCacheLocation = PlayerCameraManager->GetCameraLocation();
        TArray<FHitResult> HitResults1, HitResults2, HitResults3;

        FVector Start = GetActorLocation();
        FVector End = PlayerCameraManager->GetCameraLocation(); 

        FVector CameraRightVector = PlayerCameraManager->GetActorRightVector();
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
        TArray AllHitResults = {HitResults1, HitResults2, HitResults3};
        // Array to store all start and end vectors
        TArray<TPair<FVector, FVector>> AllVectors = {{Start1, End1}, {Start2, End2}, {Start3, End3}};

        int index = 0;
        
        bool isTwoLineHit = false;
        bool isThreeLineHit = false;
        
        if (bHit1 && bHit2 || bHit2 && bHit3 || bHit1 && bHit3)
            isTwoLineHit = true;

        if(bHit1 && bHit2 && bHit3)
            isThreeLineHit = true;
        
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

                    // Wrap the debug sphere and line commands to a console command or a debug menu
                }
                ++index;
            }
        }
    }
}

void AObstructionManager::BeginPlay()
{
	Super::BeginPlay();
}

void AObstructionManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CurrenTick++;

	if (CurrenTick >= ObstructionCheckFrequency)
	{
		if(Emitters.Num() > 0)
		{
			CheckObstruction(Emitters[CurrentEmitterIndex]);
			CurrentEmitterIndex = (CurrentEmitterIndex + 1) % Emitters.Num();
		}
	}
}

