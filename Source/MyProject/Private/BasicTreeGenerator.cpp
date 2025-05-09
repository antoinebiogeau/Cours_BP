
// BasicTreeGenerator.cpp
#include "BasicTreeGenerator.h"
#include "Components/SceneComponent.h"

ABasicTreeGenerator::ABasicTreeGenerator()
{
    PrimaryActorTick.bCanEverTick = false;

    RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
    RootComponent = RootComp;

    TrunkSpline = CreateDefaultSubobject<USplineComponent>(TEXT("TrunkSpline"));
    TrunkSpline->SetupAttachment(RootComponent);
    TrunkSpline->SetMobility(EComponentMobility::Movable);
}

void ABasicTreeGenerator::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);

    // Clear previous
    TrunkSpline->ClearSplinePoints(false);
    for (USplineMeshComponent* MeshComp : SplineMeshes)
    {
        if (MeshComp) MeshComp->DestroyComponent();
    }
    SplineMeshes.Empty();

    if (!TrunkMesh) return;

    // Build spline in local space
    FRandomStream Stream(Seed);
    FVector Pos = FVector::ZeroVector;
    FVector Dir = FVector::UpVector;
    
    for (int32 i = 0; i < NBTruncPoint; ++i)
    {
        if (i > 0)
        {
            FRotator RotRand(
                Stream.FRandRange(-Angle, Angle),
                Stream.FRandRange(-Angle, Angle),
                Stream.FRandRange(-Angle, Angle)
            );
            Dir = RotRand.RotateVector(Dir).GetSafeNormal();
            Pos += Dir * Stream.FRandRange(MinDistPointTrunc, MaxDistPointTrunc);
        }
        TrunkSpline->AddSplinePoint(Pos, ESplineCoordinateSpace::Local, false);
    }
    TrunkSpline->UpdateSpline();

    // Create mesh segments attached to spline, using Local coords
    for (int32 i = 0; i < NBTruncPoint - 1; ++i)
    {
        USplineMeshComponent* SM = NewObject<USplineMeshComponent>(this, *FString::Printf(TEXT("SplineMesh_%d"), i));
        SM->SetStaticMesh(TrunkMesh);
        SM->SetMobility(EComponentMobility::Movable);
        SM->AttachToComponent(TrunkSpline, FAttachmentTransformRules::KeepRelativeTransform);
        SM->RegisterComponentWithWorld(GetWorld());

        FVector StartLoc, StartTan, EndLoc, EndTan;
        TrunkSpline->GetLocationAndTangentAtSplinePoint(i, StartLoc, StartTan, ESplineCoordinateSpace::Local);
        TrunkSpline->GetLocationAndTangentAtSplinePoint(i + 1, EndLoc, EndTan, ESplineCoordinateSpace::Local);

        SM->SetStartAndEnd(StartLoc, StartTan, EndLoc, EndTan);

        float Alpha0 = (float)i / (NBTruncPoint - 1);
        float Alpha1 = float(i + 1) / (NBTruncPoint - 1);
        float RS = FMath::Lerp(BaseRadius, TopRadius, Alpha0);
        float RE = FMath::Lerp(BaseRadius, TopRadius, Alpha1);
        SM->SetStartScale(FVector2D(RS, RS));
        SM->SetEndScale(FVector2D(RE, RE));

        SplineMeshes.Add(SM);
    }
}
