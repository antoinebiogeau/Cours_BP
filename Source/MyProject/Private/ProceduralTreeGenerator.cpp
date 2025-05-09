
// ProceduralTreeGenerator.cpp
#include "ProceduralTreeGenerator.h"

AProceduralTreeGenerator::AProceduralTreeGenerator()
{
    PrimaryActorTick.bCanEverTick = false;
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

    TrunkSpline = CreateDefaultSubobject<USplineComponent>(TEXT("TrunkSpline"));
    TrunkSpline->SetupAttachment(RootComponent);

    InstancedMesh = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("InstancedMesh"));
    InstancedMesh->SetupAttachment(RootComponent);
}

#if WITH_EDITOR
void AProceduralTreeGenerator::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);
    bNeedsRebuild = true;
}
#endif

void AProceduralTreeGenerator::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);
    if (!bNeedsRebuild) return;
    bNeedsRebuild = false;

    ClearTree();
    BuildTrunk();
    GenerateAttractors();
    Colonize();
    BuildInstancedMeshes();
}

void AProceduralTreeGenerator::ClearTree()
{
    NodePool.Empty();
    Attractors.Empty();
    InstancedMesh->ClearInstances();
    TrunkSpline->ClearSplinePoints();
}

void AProceduralTreeGenerator::BuildTrunk()
{
    // Build trunk spline in local space
    TrunkSpline->ClearSplinePoints();
    FVector Loc(0.f);
    FVector Dir = FVector::UpVector;
    TrunkSpline->AddSplinePoint(Loc, ESplineCoordinateSpace::Local, true);

    for (int32 i = 0; i < TrunkSegments; ++i)
    {
        FRotator JR(
            FMath::FRandRange(-TrunkJitterAngle, TrunkJitterAngle),
            FMath::FRandRange(-TrunkJitterAngle, TrunkJitterAngle),
            0.f);
        Dir = JR.RotateVector(Dir).GetSafeNormal();
        float Len = FMath::FRandRange(TrunkMinSegmentLength, TrunkMaxSegmentLength);
        Loc += Dir * Len;
        TrunkSpline->AddSplinePoint(Loc, ESplineCoordinateSpace::Local, true);
    }
    TrunkSpline->UpdateSpline();

    // Seed colonization nodes along trunk
    int32 PointCount = TrunkSpline->GetNumberOfSplinePoints();
    float TotalLen = TrunkSpline->GetSplineLength();
    float MinDist = TrunkBranchStartRatio * TotalLen;

    for (int32 s = 0; s < BranchSeedCount; ++s)
    {
        float Dist = FMath::FRandRange(MinDist, TotalLen);
        FVector Pos = TrunkSpline->GetLocationAtDistanceAlongSpline(Dist, ESplineCoordinateSpace::Local);
        float Ratio = Dist / TotalLen;
        NodePool.Add({Pos, -1, 0, Ratio});
    }
}

void AProceduralTreeGenerator::GenerateAttractors()
{
    int32 Count = AttractorCount;
    Attractors.SetNumUninitialized(Count);
    FVector Center = TrunkSpline->GetLocationAtSplinePoint(TrunkSegments, ESplineCoordinateSpace::Local);

    for (int32 i = 0; i < Count; ++i)
    {
        FVector Dir = FMath::VRand();
        float r = FMath::FRandRange(0.f, CanopyRadius);
        Attractors[i] = Center + Dir * r;
    }
}

void AProceduralTreeGenerator::Colonize()
{
    int32 Iter = 0;
    while (Iter++ < MaxColonizeIterations && Attractors.Num() > 0)
    {
        TArray<TPair<int32, FVector>> Influence;
        Influence.Reserve(Attractors.Num());

        for (int32 ai = Attractors.Num() - 1; ai >= 0; --ai)
        {
            const FVector& A = Attractors[ai];
            float BestDist2 = MaxInfluenceDist * MaxInfluenceDist;
            int32  BestIdx = INDEX_NONE;

            for (int32 ni = 0; ni < NodePool.Num(); ++ni)
            {
                float d2 = FVector::DistSquared(NodePool[ni].Position, A);
                if (d2 < BestDist2)
                {
                    BestDist2 = d2;
                    BestIdx = ni;
                }
            }
            if (BestIdx == INDEX_NONE) continue;

            if (BestDist2 < MinAttractorDist * MinAttractorDist)
            {
                Attractors.RemoveAtSwap(ai);
            }
            else
            {
                FVector Dir = (A - NodePool[BestIdx].Position).GetSafeNormal();
                Influence.Emplace(BestIdx, Dir);
            }
        }

        for (auto& Pair : Influence)
        {
            FColonizationNode& Parent = NodePool[Pair.Key];
            if (Parent.Depth < BranchStartDepth || Parent.Depth >= MaxBranchDepth)
                continue;
            float MinL = GrowthMinLength * FMath::Pow(GrowthLengthFalloff, Parent.Depth);
            float MaxL = GrowthMaxLength * FMath::Pow(GrowthLengthFalloff, Parent.Depth);
            float Len  = FMath::FRandRange(MinL, MaxL);
            FVector Pos = Parent.Position + Pair.Value * Len;
            NodePool.Add({Pos, Pair.Key, Parent.Depth + 1, Parent.OriginRatio});
        }
    }
}

void AProceduralTreeGenerator::BuildInstancedMeshes()
{
    if (!SegmentMesh) return;
    InstancedMesh->SetStaticMesh(SegmentMesh);

    // Trunk segments
    int32 Count = TrunkSpline->GetNumberOfSplinePoints() - 1;
    for (int32 i = 0; i < Count; ++i)
    {
        FVector A = TrunkSpline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local);
        FVector B = TrunkSpline->GetLocationAtSplinePoint(i + 1, ESplineCoordinateSpace::Local);
        float t0   = float(i) / float(Count);
        float r0   = FMath::Lerp(TrunkBaseRadius, TrunkTopRadius, t0);
        float length = FVector::Dist(A, B);
        FVector Mid  = (A + B) * 0.5f;
        FRotator Rot = FRotationMatrix::MakeFromX(B - A).Rotator();
        FVector Scale(length, r0 * 2.f, r0 * 2.f);
        InstancedMesh->AddInstance(FTransform(Rot, Mid, Scale));
    }

    // Branch segments
    for (int32 i = 0; i < NodePool.Num(); ++i)
    {
        const FColonizationNode& N = NodePool[i];
        if (N.ParentIndex < 0) continue;
        const FVector P = NodePool[N.ParentIndex].Position;
        const FVector C = N.Position;
        float BaseR = FMath::Lerp(BranchBaseRadiusMin, BranchBaseRadiusMax, N.OriginRatio)
                      * FMath::Pow(GrowthLengthFalloff, N.Depth);
        float TipR  = BaseR * GrowthLengthFalloff;
        float length = FVector::Dist(P, C);
        FVector Mid   = (P + C) * 0.5f;
        FRotator Rot  = FRotationMatrix::MakeFromX(C - P).Rotator();
        FVector Scale(length, BaseR * 2.f, TipR * 2.f);
        InstancedMesh->AddInstance(FTransform(Rot, Mid, Scale));
    }
}
