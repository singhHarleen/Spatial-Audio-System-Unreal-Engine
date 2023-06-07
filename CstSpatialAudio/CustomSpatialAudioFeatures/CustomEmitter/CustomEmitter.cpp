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
    DrawDebugSphere(GetWorld(), GetActorLocation(), 2500, 50, FColor::Green, false, -1, 0, 20);
    
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
    FVector End = PCM->GetCameraLocation(); // For head location, add a Z offset based on your character's height
    float Distance = FVector::Dist(Start, End);

    if (Distance <= 2500)
    {
        FCollisionQueryParams TraceParams;
        TraceParams.bTraceComplex = true;
        TraceParams.bReturnPhysicalMaterial = true; 
        TraceParams.AddIgnoredActor(this);

        if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, TraceParams))
        {
            // Something is blocking the sound
            float SphereOffsetFactor = 0.2f; 
            FVector SphereLocation = FMath::Lerp(Hit.ImpactPoint, End, SphereOffsetFactor);
            DrawDebugSphere(GetWorld(), SphereLocation, 50.0f, 32, FColor::Purple, false, ObstructionCheckInterval);
            
            // Check the physical material of the hit object
            UPhysicalMaterial* HitMaterial = Hit.PhysMaterial.Get();
            if (HitMaterial != nullptr)
            {
                if (HitMaterial->GetName().Equals("Fence"))
                {
                    TargetLowPassFrequency = 20000.0f;
                }
                else if (HitMaterial->GetName().Equals("Metal"))
                {
                    TargetLowPassFrequency = 10000.0f; // Some filtering
                }
                else if (HitMaterial->GetName().Equals("Wood"))
                {
                    TargetLowPassFrequency = 1000.0f;
                }
                else if (HitMaterial->GetName().Equals("Glass"))
                {
                    TargetLowPassFrequency = 7000.0f;
                }
                else
                {
                    TargetLowPassFrequency = 100.0f;
                }
            }
        }
        else
        {
            TargetLowPassFrequency = 20000.0f;
        }
        GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ACustomEmitter::CheckObstruction);
    }
}