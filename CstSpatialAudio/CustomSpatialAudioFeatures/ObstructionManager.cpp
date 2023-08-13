#include "ObstructionManager.h"
#include "VectorTypes.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"

AObstructionManager::AObstructionManager() :
ObstructionCheckFrequency(3),
DebugSphereRadius(50.0f)
{
	PrimaryActorTick.bCanEverTick = true;
	CurrenTick = 0.0f;
}

void AObstructionManager::BeginPlay()	
{
	Super::BeginPlay();
}

void AObstructionManager::RegisterEmitter(ACustomEmitter* CustomEmitter)
{
	// This seems to reallocate the whole array when adding a new item? Maybe we need a better way to handle this 
	Emitters.Add(CustomEmitter);
}

void AObstructionManager::CheckObstruction(ACustomEmitter* CustomEmitter)
{
	for (auto Emitter : Emitters)
	{
		APlayerCameraManager* PlayerCameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);

		const float DistanceCameraEmitter = UE::Geometry::Distance(PlayerCameraManager->GetCameraLocation(),
											CustomEmitter->AudioComponent->GetComponentLocation());

		const bool IsEmitterInFallOffDistance = CustomEmitter->AudioComponent->AttenuationOverrides.FalloffDistance >= DistanceCameraEmitter;
		
		if (IsEmitterInFallOffDistance)
		{
			// if (CameraCacheLocation != PlayerCameraManager->GetCameraLocation() ||
			//    EmitterCacheLocatin != Emitter->GetActorLocation())
		 //    {
		        CameraCacheLocation = PlayerCameraManager->GetCameraLocation();
				EmitterCacheLocatin = Emitter->GetActorLocation();

				TArray<FHitResult> HitResults1, HitResults2, HitResults3, HitResults4, HitResults5;

		        FVector Start = CustomEmitter->GetActorLocation();
		        FVector End = PlayerCameraManager->GetCameraLocation();
				
		        FVector CameraRightVector = PlayerCameraManager->GetActorRightVector();
				FVector CameraUpVector = PlayerCameraManager->GetActorUpVector();
		        float OffsetFromCamera = 100.0f;

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

		        bool bHit1 = GetWorld()->LineTraceMultiByChannel(HitResults1, Start1, End1, AudioTraceChannel);
		        bool bHit2 = GetWorld()->LineTraceMultiByChannel(HitResults2, Start2, End2, AudioTraceChannel);
		        bool bHit3 = GetWorld()->LineTraceMultiByChannel(HitResults3, Start3, End3, AudioTraceChannel);
				bool bHit4 = GetWorld()->LineTraceMultiByChannel(HitResults4, Start4, End4, AudioTraceChannel);
				bool bHit5 = GetWorld()->LineTraceMultiByChannel(HitResults5, Start5, End5, AudioTraceChannel);
			
		        // Array to store all hit results
		        TArray AllHitResults = {HitResults1, HitResults2, HitResults3, HitResults4, HitResults5};
		        // Array to store all start and end vectors
		        TArray<TPair<FVector, FVector>> AllVectors = {{Start1, End1}, {Start2, End2},
		        												{Start3, End3}, {Start4, End4},
																{Start5, End5}};

		        bool isTwoLineHit = false;
		        bool isThreeLineHit = false;
		        
		        if (bHit1 && bHit2 || bHit2 && bHit3 || bHit1 && bHit3)
		            isTwoLineHit = true;

		        if(bHit1 && bHit2 && bHit3)
		            isThreeLineHit = true;
		        
		        if (isThreeLineHit)
		            TargetLowPassFrequency = MaxFiltering;
		        else if(isTwoLineHit)
		            TargetLowPassFrequency = MidFiltering;
		        else
		            TargetLowPassFrequency = NoFiltering;

    			DrawSphereAndLines = true;
		        if (isTwoLineHit && DrawSphereAndLines)
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
		                    // Wrap the debug sphere and line commands to a console command or a debug menu
		                }
		                ++Index; 
		            }
		        }
		    // }	
		}
	}
}

void AObstructionManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CurrenTick++;

	if (CurrenTick >= ObstructionCheckFrequency)
	{
		if (Emitters.Num() > 0)
		{
			CheckObstruction(Emitters[CurrentEmitterIndex]);
				CurrentEmitterIndex = (CurrentEmitterIndex + 1) % Emitters.Num();
		}
		CurrenTick = 0.0f;
	}
}