// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CustomEmitter/CustomEmitter.h"
#include "GameFramework/Actor.h"
#include "ObstructionManager.generated.h"

UCLASS()
class CSTSPATIALAUDIO_API AObstructionManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AObstructionManager();
	void RegisterEmitter(ACustomEmitter* CustomEmitter);
	void CheckObstruction(ACustomEmitter* Emitter);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	FVector CameraCacheLocation;

	UPROPERTY()
	float TargetLowPassFrequency;
	UPROPERTY()
	float InterpolationAlpha;
	UPROPERTY()
	float PrevTargetFrequency;
	UPROPERTY()
	float TransitionTime;
	bool DrawSphereAndLines = false;
	int32 ObstructionCheckFrequency; 
private:
	UPROPERTY()
	TArray<ACustomEmitter*> Emitters;
	int32 CurrentEmitterIndex;
	int32 CurrenTick;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
