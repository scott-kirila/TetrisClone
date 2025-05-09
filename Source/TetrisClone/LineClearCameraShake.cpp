// Fill out your copyright notice in the Description page of Project Settings.


#include "LineClearCameraShake.h"

#include "Shakes/PerlinNoiseCameraShakePattern.h"

ULineClearCameraShake::ULineClearCameraShake() : UCameraShakeBase(FObjectInitializer::Get())
{
	bSingleInstance = true;
	
	UPerlinNoiseCameraShakePattern* ShakePattern = CreateDefaultSubobject<UPerlinNoiseCameraShakePattern>(TEXT("ShakePattern"));

	// Location
	ShakePattern->X.Amplitude = 1.0f;
	ShakePattern->X.Frequency = 2.0f;
	ShakePattern->Y.Amplitude = 1.0f;
	ShakePattern->Y.Frequency = 2.0f;
	ShakePattern->Z.Amplitude = 1.0f;
	ShakePattern->Z.Frequency = 2.0f;
	ShakePattern->LocationAmplitudeMultiplier = 1.0f;
	ShakePattern->LocationFrequencyMultiplier = 1.0f;

	// Rotation
	ShakePattern->Pitch.Amplitude = 1.0f;
	ShakePattern->Pitch.Frequency = 2.0f;
	ShakePattern->Yaw.Amplitude = 1.0f;
	ShakePattern->Yaw.Frequency = 2.0f;
	ShakePattern->Roll.Amplitude = 1.0f;
	ShakePattern->Roll.Frequency = 2.0f;

	ShakePattern->RotationAmplitudeMultiplier = 1.0f;
	ShakePattern->RotationFrequencyMultiplier = 1.0f;

	ShakePattern->FOV.Amplitude = 1.0f;
	ShakePattern->FOV.Frequency = 2.0f;

	ShakePattern->Duration = 0.75f;
	SetRootShakePattern(ShakePattern);
}
