// Fill out your copyright notice in the Description page of Project Settings.


#include "Bp_RotatingTower_RndColors.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Engine/Engine.h"
#include "Math/UnrealMathUtility.h"
// Sets default values
ABp_RotatingTower_RndColors::ABp_RotatingTower_RndColors()
{
    PrimaryActorTick.bCanEverTick = false;
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("R"));
    SetRootComponent(RootComponent);
    ISM = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("ISM"));
    ISM->SetupAttachment(RootComponent);
    TXT = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TXT"));
    TXT->SetupAttachment(RootComponent);
    Stones_Z = 5; bRenderText = true; SM_Stone = nullptr; Material = nullptr;
}

#if WITH_EDITOR
void ABp_RotatingTower_RndColors::OnConstruction(const FTransform& T)
{
    if(Stones_Z < 1 || !SM_Stone || !Material){ ISM->ClearInstances(); return; }
    // Optionnel : vérifier SM_Stone->GetName() == "Sm_ChamferCube"
    ISM->ClearInstances();
    ISM->SetStaticMesh(SM_Stone);
    ISM->SetMaterial(0, Material);
    ISM->NumCustomDataFloats = 3;
    const float AS = (Stones_Z > 1) ? 360.f / (Stones_Z - 1) : 0.f;
    const FBoxSphereBounds B = SM_Stone->GetBounds();
    const float H = B.BoxExtent.Z * 2.f, VS = H;
    const FVector P(0.f, 0.f, B.BoxExtent.Z);
    for (int32 i = 0; i < Stones_Z; ++i)
    {
        const float A = (i == Stones_Z - 1) ? 0.f : FMath::Fmod(i * AS, 360.f);
        const FVector L = P + FVector(0.f, 0.f, i * VS);
        const FTransform X(FRotator(0.f, A, 0.f), L, FVector::OneVector);
        const int32 Idx = ISM->AddInstance(X);
        ISM->SetCustomDataValue(Idx, 0, FMath::FRand(), false);
        ISM->SetCustomDataValue(Idx, 1, FMath::FRand(), false);
        ISM->SetCustomDataValue(Idx, 2, FMath::FRand(), true);
    }
    TXT->SetText(FText::FromString(FString::Printf(TEXT("Cube Count: %d\nBlueprint Name: %s"), Stones_Z, *GetFName().ToString())));
    TXT->SetVisibility(bRenderText);
}
#endif

void ABp_RotatingTower_RndColors::BeginPlay(){ Super::BeginPlay(); }
void ABp_RotatingTower_RndColors::Tick(float D){ Super::Tick(D); }

