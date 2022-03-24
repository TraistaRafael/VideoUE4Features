/*
MIT License

Copyright (c) 2022 Rafael Traista

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

Authors:
Traista Rafael <traista.rafael@yahoo.com>
*/

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PerceptionVisualiserGameModeBase.generated.h"

UCLASS()
class PERCEPTIONVISUALISER_API APerceptionVisualiserGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	APerceptionVisualiserGameModeBase(const FObjectInitializer& ObjectIn);
	
	virtual void BeginPlay();

	virtual void Tick(float DeltaSeconds);

	void ReadMotionData();

	/**
	* OpenCV expands objects too much on Z, making the 3D visualisation look bad. 
	* This functions shrinks points in 3D space on Z axis (Z axis in openCV = Y axis in UE4)
	*/
	void ShrinkMotionObject(TArray<FVector>& motionPoints);

	/**
	* Bring motion object with feet on ground
	* Find minimum Z value and move all points according
	*/
	void BringMotionDataOnGround(TArray<FVector>& motionPoints);

	long Frame = 0;
	long Index = 0;
	TArray<TArray<FVector>> MotionPoints;	
	TMap<int, TArray<int>> BoneMap;
};
