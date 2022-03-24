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

#include "PerceptionVisualiserGameModeBase.h"
#include "Json.h"
#include "DrawDebugHelpers.h"
#include "GenericPlatform/GenericPlatformProcess.h"
#include "Misc/Paths.h"
//#include "FileManager.h"

#define MOTION_DATA_JSON "C:/Projects/UE4VideoFeatures/shared/motion_data.json"

APerceptionVisualiserGameModeBase::APerceptionVisualiserGameModeBase(const FObjectInitializer& ObjectIn) : AGameModeBase(ObjectIn)
{
	UE_LOG(LogTemp, Display, TEXT("APerceptionVisualiserGameModeBase Constructor"));
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;
    PrimaryActorTick.bAllowTickOnDedicatedServer = true;

    // Head
    BoneMap.Add(0, { 4 });
    BoneMap.Add(4, { 5 });
    BoneMap.Add(5, { 6 });
    BoneMap.Add(6, { 8 });
    BoneMap.Add(0, { 1 });
    BoneMap.Add(1, { 2 });
    BoneMap.Add(2, { 3 });
    BoneMap.Add(3, { 7 });
    BoneMap.Add(9, { 10 });

    // Body
    BoneMap.Add(11, {12, 23, 13});
    BoneMap.Add(12, {12, 23, 14});
    BoneMap.Add(24, {12, 23, 26});
    BoneMap.Add(23, {11, 24, 25});

    // Right hand
    BoneMap.Add(13, {15});
    BoneMap.Add(15, {21, 19, 17});
    BoneMap.Add(19, {17});

    // Left hand
    BoneMap.Add(14, {16});
    BoneMap.Add(16, {22, 20, 18});
    BoneMap.Add(18, {20});

    // Left leg
    BoneMap.Add(26, {28});
    BoneMap.Add(28, {30, 32});
    BoneMap.Add(30, {32});

    // Right leg
    BoneMap.Add(25, {27});
    BoneMap.Add(27, {29, 31});
    BoneMap.Add(29, {31});

}

void APerceptionVisualiserGameModeBase::Tick(float DeltaSeconds)
{
    UE_LOG(LogTemp, Display, TEXT("APerceptionVisualiserGameModeBas::Tick"));
    FVector origin(-400, -600, 0);

    if (Frame % 5 == 0) 
    {
        FlushPersistentDebugLines(GetWorld());
        TArray<FVector> FramePoints = MotionPoints[Index];

        for (int j = 0; j < FramePoints.Num(); j++) {
            DrawDebugPoint(
                GetWorld(), 
                origin + FramePoints[j], 
                10, 
                FColor(255, 0, 0), 
                true,
                100,
                255
            );

            if (BoneMap.Contains(j)) {
                for (int k = 0; k < BoneMap[j].Num(); k++) {
                    DrawDebugLine(
                        GetWorld(),
                        origin + FramePoints[j],
                        origin + FramePoints[BoneMap[j][k]],
                        FColor(0, 255, 0),
                        true,
                        255,
                        7);
                }     
            }
        }
       
        Index++;
        if (Index >= MotionPoints.Num()) {
            Index = 0;
        }
    }

    Frame++;
}

void APerceptionVisualiserGameModeBase::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Display, TEXT("APerceptionVisualiserGameModeBas::BeginPlay()"));
    ReadMotionData();
}

void APerceptionVisualiserGameModeBase::ShrinkMotionObject(TArray<FVector> &motionPoints)
{
    if (motionPoints.Num() == 0) return;

    float minY = motionPoints[0].Y;
    float maxY = motionPoints[0].Y;

    for (int i = 0; i < motionPoints.Num(); i++) {
        if (motionPoints[i].Y > maxY) {
            maxY = motionPoints[i].Y;
        }
        if (motionPoints[i].Y < minY) {
            minY = motionPoints[i].Y;
        }
    }

    // Make motions points 10x thiner
    float maxExpand = FMath::Abs(maxY - minY) / 10;
    for (int i = 0; i < motionPoints.Num(); i++) {
        motionPoints[i].Y = ((motionPoints[i].Y - minY) / (maxY - minY)) * maxExpand;
    }
}

void APerceptionVisualiserGameModeBase::BringMotionDataOnGround(TArray<FVector>& motionPoints)
{
    if (motionPoints.Num() == 0) return;

    float minZ = motionPoints[0].Z;

    for (int i = 0; i < motionPoints.Num(); i++) {
        if (motionPoints[i].Z < minZ) {
            minZ = motionPoints[i].Z;
        }
    }

    for (int i = 0; i < motionPoints.Num(); i++) {
        motionPoints[i].Z -= minZ;
    }
}

void APerceptionVisualiserGameModeBase::ReadMotionData()
{
    TSharedPtr<FJsonObject> jsonObject = MakeShareable(new FJsonObject);

    FString jsonStr;
    FFileHelper::LoadFileToString(jsonStr, TEXT(MOTION_DATA_JSON));

    TSharedRef<TJsonReader<>> jsonReader = TJsonReaderFactory<>::Create(jsonStr);

    if (!FJsonSerializer::Deserialize(jsonReader, jsonObject))
    {
        UE_LOG(LogTemp, Error, TEXT("APerceptionVisualiserGameModeBase::ReadMotionData() Json deserialize failed with status"));
        return;
    }

    TArray<TSharedPtr<FJsonValue>> framesArray = jsonObject->GetArrayField(FString("data"));
    for (int i=0; i<framesArray.Num(); i++) 
    {
        TArray<FVector> FramePoints;

        if (framesArray[i]->IsNull())
        {
            MotionPoints.Add(FramePoints);
            continue;
        }

        TArray<TSharedPtr<FJsonValue>> lmList = framesArray[i]->AsObject()->GetArrayField("lmList");
        for (int j = 0; j < lmList.Num(); j++) {
            double x = lmList[j]->AsArray()[1]->AsNumber();
            double y = lmList[j]->AsArray()[2]->AsNumber();
            double z = lmList[j]->AsArray()[3]->AsNumber();
            FVector location(x, z, y);
            FramePoints.Add(location);
        }

        ShrinkMotionObject(FramePoints);
        BringMotionDataOnGround(FramePoints);
        MotionPoints.Add(FramePoints);
    }
    
}
