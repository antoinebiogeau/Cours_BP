// Fill out your copyright notice in the Description page of Project Settings.


#include "Chess.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Engine/Engine.h"
// Sets default values
AChess::AChess()
{
    PrimaryActorTick.bCanEverTick = false;
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
    SetRootComponent(RootComponent);
    InstancedMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("InstancedMesh"));
    InstancedMesh->SetupAttachment(RootComponent);
    InfoText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("InfoText"));
    InfoText->SetupAttachment(RootComponent);
    bRenderText = true;
    SM_Stone = nullptr;
    Material = nullptr;
}

#if WITH_EDITOR
void AChess::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);
    InstancedMesh->ClearInstances();
    if (!SM_Stone || !Material)
    {
        return;
    }
    InstancedMesh->SetStaticMesh(SM_Stone);
    InstancedMesh->SetMaterial(0, Material);
    const int32 GridSize = 8;
    const float Spacing = 100.f;
    InstancedMesh->NumCustomDataFloats = 3;
    FVector LocalPivotOffset(-400.f, 0.f, -400.f);
    for (int32 i = 0; i < GridSize; ++i)
    {
        for (int32 j = 0; j < GridSize; ++j)
        {
            FVector Location = FVector(i * Spacing, 0.f, j * Spacing) + LocalPivotOffset;
            FTransform InstanceTransform;
            InstanceTransform.SetLocation(Location);
            InstanceTransform.SetRotation(FRotator::ZeroRotator.Quaternion());
            InstanceTransform.SetScale3D(FVector(1.f));

            int32 InstanceIndex = InstancedMesh->AddInstance(InstanceTransform);

            // Déterminer la couleur pour le damier : blanc si (i+j) pair, sinon noir
            bool bIsWhite = ((i + j) % 2 == 0);
            float ColorValue = bIsWhite ? 1.0f : 0.0f;
            InstancedMesh->SetCustomDataValue(InstanceIndex, 0, ColorValue, true);
            InstancedMesh->SetCustomDataValue(InstanceIndex, 1, ColorValue, true);
            InstancedMesh->SetCustomDataValue(InstanceIndex, 2, ColorValue, true);
        }
    }
    int32 CubeCount = GridSize * GridSize; // 64
    FString BlueprintName = GetFName().ToString();
    FString InfoString = FString::Printf(TEXT("Cube Count: %d\nBlueprint Name: %s"),
                                         CubeCount, *BlueprintName);

    InfoText->SetText(FText::FromString(InfoString));
    InfoText->SetVisibility(bRenderText);
}

void AChess::BeginPlay()
{
    Super::BeginPlay();
}

void AChess::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}
#endif

