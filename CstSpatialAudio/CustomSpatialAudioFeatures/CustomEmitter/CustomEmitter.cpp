// Fill out your copyright notice in the Description page of Project Settings.
#include "CustomEmitter.h"
#include "CoreMinimal.h"
#include "MathUtil.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "Components/AudioComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

ACustomEmitter::ACustomEmitter()
{
	PrimaryActorTick.bCanEverTick = true;
	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));

    TargetLowPassFrequency = 20000.0f;
    InterpolationAlpha = 0.0f;
    prevTargetFrequency = 0.0f;
}
void ACustomEmitter::BeginPlay()
{
    Super::BeginPlay();

    GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ACustomEmitter::CheckObstruction);
    AudioComponent->SetLowPassFilterEnabled(true);
}

void ACustomEmitter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    DrawDebugSphere(GetWorld(), GetActorLocation(), AudioComponent->AttenuationOverrides.FalloffDistance, 50,
                    FColor::Green, false, -1, 0, 1);
    
    elapsedTime += DeltaTime;
    float LerpRatio = FMath::Clamp(elapsedTime / transitionTime, 0.0f, 1.0f);
    prevTargetFrequency = FMath::Lerp(prevTargetFrequency, TargetLowPassFrequency, LerpRatio);
    AudioComponent->SetLowPassFilterFrequency(prevTargetFrequency);

    if (LerpRatio >= 1.0f)
    {
        elapsedTime = 0.0f;
    }

    UE_LOG(LogTemp, Warning, TEXT("Current Frequency: %f"), prevTargetFrequency);
}

void ACustomEmitter::CheckObstruction()
{
    FHitResult Hit;
    FVector Start = GetActorLocation();

    APlayerCameraManager* PCM = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
    FVector End = PCM->GetCameraLocation(); 
    float Distance = FVector::Dist(Start, End);

    if (Distance <= 2500) // Need to get the fall off distance here
    {
        FCollisionQueryParams TraceParams;
        TraceParams.bTraceComplex = true;
        TraceParams.bReturnPhysicalMaterial = true; 
        TraceParams.AddIgnoredActor(this);

        if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, TraceParams))
        {
            float SphereOffsetFactor = 0.2f; 
            FVector SphereLocation = FMath::Lerp(Hit.ImpactPoint, End, SphereOffsetFactor);
            DrawDebugSphere(GetWorld(), SphereLocation, 50.0f, 32,
                            FColor::Purple, false, ObstructionCheckInterval);

            // Check the physical material of the hit object
            UPhysicalMaterial* HitMaterial = Hit.PhysMaterial.Get();
            FColor DebugLineColor = FColor::Red;
            if (HitMaterial != nullptr)
            {
                
                if (HitMaterial->GetName().Equals("Metal"))
                {
                    TargetLowPassFrequency = 10000.0f;
                    DebugLineColor = FColor::Green;
                }
                else if (HitMaterial->GetName().Equals("Wood"))
                {
                    TargetLowPassFrequency = 1000.0f;
                    DebugLineColor = FColor::Green;
                }
                else if (HitMaterial->GetName().Equals("Concrete"))
                {
                    TargetLowPassFrequency = 7000.0f;
                    DebugLineColor = FColor::Green;
                }
                
                FString DebugText = FString::Printf(TEXT("Material: %s"), *HitMaterial->GetName());
                
                DrawDebugString(GetWorld(), SphereLocation + FVector(0,0,100), DebugText, nullptr,
                                FColor::Yellow, ObstructionCheckInterval);
            }
            
            // DrawDebugLine(GetWorld(), Start, End, DebugLineColor, false,
            //        ObstructionCheckInterval, 0, 0.1);
        }
        else
        {
            TargetLowPassFrequency = 20000.0f;
        }
        GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ACustomEmitter::CheckObstruction);
    }
}
