// Fill out your copyright notice in the Description page of Project Settings.


#include "treeGen.h"
#include "Engine/World.h"


AtreeGen::AtreeGen()
{
    PrimaryActorTick.bCanEverTick = false;
    TrunkSpline = CreateDefaultSubobject<USplineComponent>(TEXT("TrunkSpline"));
    SetRootComponent(TrunkSpline);
}

void AtreeGen::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);
    ClearPrevious();
    RandomStream.Initialize(Seed);
    GenerateTrunk();
    GenerateBranches();
}

void AtreeGen::ClearPrevious()
{
    TrunkSpline->ClearSplinePoints();
    for (auto* Seg : TrunkMeshSegments) if (Seg) Seg->DestroyComponent();
    for (auto* Spl : BranchSplines) if (Spl) Spl->DestroyComponent();
    for (auto* Seg : BranchMeshSegments) if (Seg) Seg->DestroyComponent();

    TrunkMeshSegments.Empty();
    BranchSplines.Empty();
    BranchMeshSegments.Empty();
}

void AtreeGen::GenerateTrunk()
{
    FVector CurrPos = FVector::ZeroVector;
    FVector CurrDir = FVector::UpVector;
    TrunkSpline->AddSplinePoint(CurrPos, ESplineCoordinateSpace::Local);

    for (int32 i = 1; i < NumTrunkPoints; ++i)
    {
        float Dist = RandomStream.FRandRange(MinPointDistance, MaxPointDistance);
        FRotator Bend(
            RandomStream.FRandRange(-MaxTrunkBendAngle, MaxTrunkBendAngle),
            RandomStream.FRandRange(-MaxTrunkBendAngle, MaxTrunkBendAngle),
            0.f);
        CurrDir = Bend.RotateVector(CurrDir).GetSafeNormal();
        CurrPos += CurrDir * Dist;
        TrunkSpline->AddSplinePoint(CurrPos, ESplineCoordinateSpace::Local);
    }
    TrunkSpline->SetClosedLoop(false);

    int32 Count = TrunkSpline->GetNumberOfSplinePoints();
    for (int32 i = 0; i < Count - 1; ++i)
    {
        USplineMeshComponent* Seg = NewObject<USplineMeshComponent>(this, *FString::Printf(TEXT("TrunkSeg_%d"), i));
        AddInstanceComponent(Seg);
        Seg->SetStaticMesh(TrunkMesh);
        Seg->SetupAttachment(TrunkSpline);
        Seg->RegisterComponent();

        FVector StartPos, StartTan, EndPos, EndTan;
        TrunkSpline->GetLocationAndTangentAtSplinePoint(i, StartPos, StartTan, ESplineCoordinateSpace::Local);
        TrunkSpline->GetLocationAndTangentAtSplinePoint(i+1, EndPos, EndTan, ESplineCoordinateSpace::Local);
        Seg->SetStartAndEnd(StartPos, StartTan, EndPos, EndTan);

        float Alpha = float(i) / (Count - 1);
        float S0 = FMath::Lerp(TrunkBaseScale, TrunkTipScale, Alpha);
        float S1 = FMath::Lerp(TrunkBaseScale, TrunkTipScale, Alpha + 1.f/(Count - 1));
        Seg->SetStartScale(FVector2D(S0));
        Seg->SetEndScale(FVector2D(S1));

        Seg->SetForwardAxis(ESplineMeshAxis::X);
        Seg->SetSplineUpDir(FVector::UpVector, ESplineCoordinateSpace::Local);

        TrunkMeshSegments.Add(Seg);
    }
}

void AtreeGen::GenerateBranches()
{
    float TotalLen = TrunkSpline->GetSplineLength();
    for (int32 i = 0; i < NumBranches; ++i)
    {
        float DistOnTrunk = RandomStream.FRandRange(BranchSpawnMinHeight, TotalLen);
        FVector WorldPos = TrunkSpline->GetLocationAtDistanceAlongSpline(DistOnTrunk, ESplineCoordinateSpace::World);
        FVector WorldTan = TrunkSpline->GetTangentAtDistanceAlongSpline(DistOnTrunk, ESplineCoordinateSpace::World).GetSafeNormal();

        FRotator RandRot(
            RandomStream.FRandRange(-45.f,45.f),
            RandomStream.FRandRange(0.f,360.f),
            0.f);
        FVector Dir = RandRot.RotateVector(WorldTan);
        float Len = RandomStream.FRandRange(BranchMinLength, BranchMaxLength);
        FVector WorldEnd = WorldPos + Dir * Len;

        USplineComponent* BranchSpl = NewObject<USplineComponent>(this, *FString::Printf(TEXT("BranchSpline_%d"), i));
        AddInstanceComponent(BranchSpl);
        BranchSpl->SetupAttachment(TrunkSpline);
        BranchSpl->RegisterComponent();

        FVector LocalStart = BranchSpl->GetComponentTransform().InverseTransformPosition(WorldPos);
        FVector LocalEnd   = BranchSpl->GetComponentTransform().InverseTransformPosition(WorldEnd);
        BranchSpl->AddSplinePoint(LocalStart, ESplineCoordinateSpace::Local);
        BranchSpl->AddSplinePoint(LocalEnd,   ESplineCoordinateSpace::Local);
        BranchSpl->SetClosedLoop(false);

        FVector BSPos = BranchSpl->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::Local);
        FVector BSTan = BranchSpl->GetTangentAtSplinePoint(0, ESplineCoordinateSpace::Local).GetSafeNormal();
        FVector BEPos = BranchSpl->GetLocationAtSplinePoint(1, ESplineCoordinateSpace::Local);
        FVector BETan = BranchSpl->GetTangentAtSplinePoint(1, ESplineCoordinateSpace::Local).GetSafeNormal();

        USplineMeshComponent* Seg = NewObject<USplineMeshComponent>(this, *FString::Printf(TEXT("BranchSeg_%d"), i));
        AddInstanceComponent(Seg);
        Seg->SetStaticMesh(BranchMesh);
        Seg->SetupAttachment(BranchSpl);
        Seg->RegisterComponent();

        Seg->SetStartAndEnd(BSPos, BSTan, BEPos, BETan);
        Seg->SetStartScale(FVector2D(BranchBaseScale));
        Seg->SetEndScale(FVector2D(BranchTipScale));
        Seg->SetForwardAxis(ESplineMeshAxis::X);
        Seg->SetSplineUpDir(FVector::UpVector, ESplineCoordinateSpace::Local);

        BranchSplines.Add(BranchSpl);
        BranchMeshSegments.Add(Seg);
    }
}
