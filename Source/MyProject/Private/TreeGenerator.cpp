// Fill out your copyright notice in the Description page of Project Settings.


#include "TreeGenerator.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"


ATreeGenerator::ATreeGenerator()
{
    PrimaryActorTick.bCanEverTick = false;
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    TrunkSpline = CreateDefaultSubobject<USplineComponent>(TEXT("TrunkSpline"));
    TrunkSpline->SetupAttachment(RootComponent);
}

void ATreeGenerator::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);
    ClearTree();
    BuildTrunk();
    GenerateAttractors();
    Colonize();
    BuildSplines();
}

void ATreeGenerator::ClearTree()
{
    // on supprime les splines spline mesh et la colonisation pour eviter les bug(sans ca j'avais des spline qui apparraissait de mariniere exponential a chaque fois
    TArray<USplineComponent*> Splines;
    GetComponents<USplineComponent>(Splines);
    for (USplineComponent* S : Splines)
    {
        if (S != TrunkSpline) S->DestroyComponent();
    }
    
    TArray<USplineMeshComponent*> MeshComps;
    GetComponents<USplineMeshComponent>(MeshComps);
    for (USplineMeshComponent* M : MeshComps)
    {
        M->DestroyComponent();
    }
    for (FColonizationNode* N : Nodes) delete N;
    Nodes.Empty();
    Attractors.Empty();
}

void ATreeGenerator::BuildTrunk()
{
    TrunkSpline->ClearSplinePoints();
    FVector Loc = FVector::ZeroVector;
    FVector Dir = FVector::UpVector;
    TrunkSpline->AddSplinePoint(Loc, ESplineCoordinateSpace::World, true);
    for (int32 i = 0; i < TrunkSegments; ++i)
    {
        // Jitter pour donner de la variation au tronc
        FRotator JR(
            FMath::FRandRange(-TrunkJitterAngle, TrunkJitterAngle),
            FMath::FRandRange(-TrunkJitterAngle, TrunkJitterAngle),
            0.f);
        Dir = JR.RotateVector(Dir).GetSafeNormal();
        // Random sur la longueur des segments
        float Len = FMath::FRandRange(TrunkMinSegmentLength, TrunkMaxSegmentLength);
        Loc += Dir * Len;
        TrunkSpline->AddSplinePoint(Loc, ESplineCoordinateSpace::World, true);
    }
    TrunkSpline->UpdateSpline();
    
    int32 Count = TrunkSpline->GetNumberOfSplinePoints() - 1;
    for (int32 i = 0; i < Count; ++i)
    {
        FVector A = TrunkSpline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::World);
        FVector B = TrunkSpline->GetLocationAtSplinePoint(i+1, ESplineCoordinateSpace::World);
        float t0 = float(i) / float(Count);
        float t1 = float(i+1) / float(Count);
        float r0 = FMath::Lerp(TrunkBaseRadius, TrunkTopRadius, t0);
        float r1 = FMath::Lerp(TrunkBaseRadius, TrunkTopRadius, t1);
        CreateSplineSegment(A, B, r0, r1);
    }
    {
        float totalLength = TrunkSpline->GetSplineLength();
        float minDist = TrunkBranchStartRatio * totalLength;
        for (int32 s = 0; s < BranchSeedCount; ++s)
        {
            float dist = FMath::FRandRange(minDist, totalLength);
            FVector SeedPos = TrunkSpline->GetLocationAtDistanceAlongSpline(dist, ESplineCoordinateSpace::World);
            Nodes.Add(new FColonizationNode(SeedPos, nullptr, 0));
        }
    }
    }

void ATreeGenerator::GenerateAttractors()
{
    FVector Center = TrunkSpline->GetLocationAtSplinePoint(TrunkSegments, ESplineCoordinateSpace::World);
    Attractors.Reserve(AttractorCount);
    for (int32 i = 0; i < AttractorCount; ++i)
    {
        FVector Dir = FMath::VRand();
        float r = FMath::FRandRange(0.f, CanopyRadius);
        Attractors.Add(Center + Dir * r);
    }
}

void ATreeGenerator::Colonize()
{
    int32 Iter = 0;
    while (Iter++ < MaxColonizeIterations && Attractors.Num())
    {
        TMap<FColonizationNode*, FVector> InfluenceMap;
        for (int32 ai = Attractors.Num()-1; ai >= 0; --ai)
        {
            const FVector& A = Attractors[ai];
            float best = MaxInfluenceDist * MaxInfluenceDist;
            FColonizationNode* bestNode = nullptr;
            for (FColonizationNode* N : Nodes)
            {
                float d2 = FVector::DistSquared(N->Position, A);
                if (d2 < best)
                {
                    best = d2;
                    bestNode = N;
                }
            }
            if (!bestNode) continue;
            if (best < MinAttractorDist * MinAttractorDist)
            {
                Attractors.RemoveAtSwap(ai);
            }
            else
            {
                InfluenceMap.FindOrAdd(bestNode) += (A - bestNode->Position).GetSafeNormal();
            }
        }
        for (auto& P : InfluenceMap)
        {
            FColonizationNode* parent = P.Key;
            int32 depth = parent->Depth;
            if (depth < BranchStartDepth || depth >= MaxBranchDepth)
                continue;
            FVector dir = P.Value.GetSafeNormal();
            float minL = GrowthMinLength * FMath::Pow(GrowthLengthFalloff, depth);
            float maxL = GrowthMaxLength * FMath::Pow(GrowthLengthFalloff, depth);
            float Len = FMath::FRandRange(minL, maxL);
            FVector pos = parent->Position + dir * Len;
            auto* child = new FColonizationNode(pos, parent, depth+1);
            parent->Children.Add(child);
            Nodes.Add(child);
        }
    }
}

void ATreeGenerator::BuildSplines()
{
    for (FColonizationNode* N : Nodes)
    {
        if (!N->Parent) continue;
        float r0 = TrunkTopRadius * FMath::Pow(GrowthLengthFalloff, N->Depth);
        float r1 = r0 * GrowthLengthFalloff;
        CreateSplineSegment(N->Parent->Position, N->Position, r0, r1);
    }
}

void ATreeGenerator::CreateSplineSegment(const FVector& A, const FVector& B, float R0, float R1)
{
    if (!SegmentMesh) return;
    FTransform ActorTransform = GetActorTransform();
    FVector LocalA = ActorTransform.InverseTransformPosition(A);
    FVector LocalB = ActorTransform.InverseTransformPosition(B);

    USplineComponent* Spline = NewObject<USplineComponent>(this, USplineComponent::StaticClass());
    Spline->RegisterComponentWithWorld(GetWorld());
    Spline->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
    Spline->ClearSplinePoints();
    Spline->AddSplinePoint(LocalA, ESplineCoordinateSpace::World, true);
    Spline->AddSplinePoint(LocalB, ESplineCoordinateSpace::World, true);
    Spline->UpdateSpline();


    USplineMeshComponent* MeshComp = NewObject<USplineMeshComponent>(this, USplineMeshComponent::StaticClass());
    MeshComp->RegisterComponentWithWorld(GetWorld());
    MeshComp->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
    MeshComp->SetStaticMesh(SegmentMesh);


    FVector StartPos = Spline->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World);
    FVector EndPos   = Spline->GetLocationAtSplinePoint(1, ESplineCoordinateSpace::World);
    FVector StartTan = Spline->GetTangentAtSplinePoint(0, ESplineCoordinateSpace::World) * TangentScale;
    FVector EndTan   = Spline->GetTangentAtSplinePoint(1, ESplineCoordinateSpace::World) * TangentScale;


    MeshComp->SetStartAndEnd(StartPos, StartTan, EndPos, EndTan);
    MeshComp->SetStartRoll(0.f);
    MeshComp->SetEndRoll(0.f);
    MeshComp->SetStartScale(FVector2D(R0, R0));
    MeshComp->SetEndScale(FVector2D(R1, R1));
}
