// BasicTreeGenerator.cpp
#include "BasicTreeGenerator.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"

ABasicTreeGenerator::ABasicTreeGenerator()
{
    PrimaryActorTick.bCanEverTick = false;
    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = Root;
    TrunkSpline = CreateDefaultSubobject<USplineComponent>(TEXT("TrunkSpline"));
    TrunkSpline->SetupAttachment(Root);
}

#if WITH_EDITOR
void ABasicTreeGenerator::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);
    ClearAllComponents();
    BuildTrunk();
    BuildBranches();
}
#endif

void ABasicTreeGenerator::ClearAllComponents()
{
    if (TrunkSpline) TrunkSpline->ClearSplinePoints();

    for (auto* Comp : MeshComponents)
        if (Comp) Comp->DestroyComponent();
    MeshComponents.Empty();

    for (auto* Spline : BranchSplines)
        if (Spline) Spline->DestroyComponent();
    BranchSplines.Empty();
}

void ABasicTreeGenerator::BuildTrunk()
{
    if (!TrunkSpline) return;
    FRandomStream Stream; Stream.GenerateNewSeed();
    TrunkSpline->SetClosedLoop(false);

    // Points
    for (int32 i = 0; i <= TrunkPointCount; ++i)
    {
        FVector Loc = FVector::ZeroVector;
        if (i > 0)
        {
            FVector Prev = TrunkSpline->GetLocationAtSplinePoint(i - 1, ESplineCoordinateSpace::Local);
            float Len = Stream.FRandRange(MinTrunkSegmentLength, MaxTrunkSegmentLength);
            FRotator J(Stream.FRandRange(-TrunkJitterAngle, TrunkJitterAngle),
                       Stream.FRandRange(-TrunkJitterAngle, TrunkJitterAngle),
                       Stream.FRandRange(-TrunkJitterAngle, TrunkJitterAngle));
            FVector Dir = (i == 1) ? FVector::UpVector : TrunkSpline->GetTangentAtSplinePoint(i - 1, ESplineCoordinateSpace::Local).GetSafeNormal();
            Loc = Prev + J.RotateVector(Dir) * Len;
        }
        TrunkSpline->AddSplinePoint(Loc, ESplineCoordinateSpace::Local);
    }
    TrunkSpline->UpdateSpline();

    // Mesh
    for (int32 i = 0; i < TrunkPointCount; ++i)
    {
        auto* M = NewObject<USplineMeshComponent>(this);
        M->SetMobility(EComponentMobility::Movable);
        M->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
        M->SetStaticMesh(SegmentMesh);
        M->RegisterComponent();

        FVector S = TrunkSpline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local);
        FVector E = TrunkSpline->GetLocationAtSplinePoint(i+1, ESplineCoordinateSpace::Local);
        FVector ST = TrunkSpline->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::Local) * TangentScale;
        FVector ET = TrunkSpline->GetTangentAtSplinePoint(i+1, ESplineCoordinateSpace::Local) * TangentScale;
        M->SetStartAndEnd(S, ST, E, ET);
        MeshComponents.Add(M);
    }
}

void ABasicTreeGenerator::BuildBranches()
{
    if (!TrunkSpline) return;
    FRandomStream Stream; Stream.GenerateNewSeed();
    int32 StartIdx = FMath::FloorToInt(BranchStartRatio * TrunkPointCount);

    for (int32 b = 0; b < BranchCount; ++b)
    {
        int32 Idx = Stream.RandRange(StartIdx, TrunkPointCount-1);
        FVector Base = TrunkSpline->GetLocationAtSplinePoint(Idx, ESplineCoordinateSpace::Local);
        FVector RandDir = Stream.VRand().GetSafeNormal();
        float Inf = FMath::Clamp(BranchTangentInfluence, -1.f,1.f);
        FVector Tan = TrunkSpline->GetTangentAtSplinePoint(Idx, ESplineCoordinateSpace::Local).GetSafeNormal();
        FVector Blended = FMath::Lerp(RandDir, Inf<0? -Tan:Tan, FMath::Abs(Inf)).GetSafeNormal();

        USplineComponent* Spline = NewObject<USplineComponent>(this);
        Spline->RegisterComponent();
        Spline->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
        Spline->ClearSplinePoints();

        FVector Dir = Blended;
        for (int32 i=0;i<=BranchPointCount;++i)
        {
            FVector P = (i==0?Base: Spline->GetLocationAtSplinePoint(i-1,ESplineCoordinateSpace::Local));
            if(i>0)
            {
                FRotator J(Stream.FRandRange(-BranchJitterAngle,BranchJitterAngle),
                           Stream.FRandRange(-BranchJitterAngle,BranchJitterAngle),
                           Stream.FRandRange(-BranchJitterAngle,BranchJitterAngle));
                Dir = J.RotateVector(Dir).GetSafeNormal();
                P += Dir * Stream.FRandRange(MinBranchLength,MaxBranchLength);
            }
            Spline->AddSplinePoint(P,ESplineCoordinateSpace::Local);
        }
        Spline->UpdateSpline();
        BranchSplines.Add(Spline);

        for(int32 i=0;i<BranchPointCount;++i)
        {
            auto* M = NewObject<USplineMeshComponent>(this);
            M->SetMobility(EComponentMobility::Movable);
            M->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
            M->SetStaticMesh(SegmentMesh);
            M->RegisterComponent();
            FVector S = Spline->GetLocationAtSplinePoint(i,ESplineCoordinateSpace::Local);
            FVector E = Spline->GetLocationAtSplinePoint(i+1,ESplineCoordinateSpace::Local);
            FVector ST = Spline->GetTangentAtSplinePoint(i,ESplineCoordinateSpace::Local)*TangentScale;
            FVector ET = Spline->GetTangentAtSplinePoint(i+1,ESplineCoordinateSpace::Local)*TangentScale;
            M->SetStartAndEnd(S,ST,E,ET);
            float A0=float(i)/BranchPointCount, A1=float(i+1)/BranchPointCount;
            M->SetStartScale(FVector2D(FMath::Lerp(BranchBaseRadius,BranchTopRadius,A0)));
            M->SetEndScale  (FVector2D(FMath::Lerp(BranchBaseRadius,BranchTopRadius,A1)));
            MeshComponents.Add(M);
        }
        GenerateSubBranches(Spline,0);
    }
}

void ABasicTreeGenerator::GenerateSubBranches(USplineComponent* Parent, int32 Depth)
{
    if(Depth>2) return;
    FRandomStream Stream; Stream.GenerateNewSeed();
    float LenTot = Parent->GetSplineLength();
    FVector Prev = Parent->GetTangentAtSplinePoint(Parent->GetNumberOfSplinePoints()-2,ESplineCoordinateSpace::Local).GetSafeNormal();

    for(int32 b=0;b<SubBranchCount;++b)
    {
        float D = Stream.FRand()*LenTot;
        FVector Base = Parent->GetLocationAtDistanceAlongSpline(D,ESplineCoordinateSpace::Local);
        FVector Dir = FMath::Lerp(Stream.VRand().GetSafeNormal(),Prev,0.5f).GetSafeNormal();

        USplineComponent* S = NewObject<USplineComponent>(this);
        S->RegisterComponent();
        S->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
        S->ClearSplinePoints();

        for(int32 i=0;i<=SubBranchPointCount;++i)
        {
            FVector P = (i==0?Base:S->GetLocationAtSplinePoint(i-1,ESplineCoordinateSpace::Local));
            if(i>0)
            {
                FRotator J(Stream.FRandRange(-SubBranchJitterAngle,SubBranchJitterAngle),
                           Stream.FRandRange(-SubBranchJitterAngle,SubBranchJitterAngle),
                           Stream.FRandRange(-SubBranchJitterAngle,SubBranchJitterAngle));
                Dir = J.RotateVector(Dir).GetSafeNormal();
                P += Dir * Stream.FRandRange(MinSubBranchLength,MaxSubBranchLength);
            }
            S->AddSplinePoint(P,ESplineCoordinateSpace::Local);
        }
        S->UpdateSpline();

        for(int32 i=0;i<SubBranchPointCount;++i)
        {
            auto* M = NewObject<USplineMeshComponent>(this);
            M->SetMobility(EComponentMobility::Movable);
            M->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
            M->SetStaticMesh(SegmentMesh);
            M->RegisterComponent();
            FVector Spos=S->GetLocationAtSplinePoint(i,ESplineCoordinateSpace::Local);
            FVector Epos=S->GetLocationAtSplinePoint(i+1,ESplineCoordinateSpace::Local);
            FVector StartT=S->GetTangentAtSplinePoint(i,ESplineCoordinateSpace::Local)*TangentScale;
            FVector EndT=S->GetTangentAtSplinePoint(i+1,ESplineCoordinateSpace::Local)*TangentScale;
            M->SetStartAndEnd(Spos,StartT,Epos,EndT);
            float a0=float(i)/SubBranchPointCount,a1=float(i+1)/SubBranchPointCount;
            M->SetStartScale(FVector2D(FMath::Lerp(SubBranchBaseRadius,SubBranchTopRadius,a0)));
            M->SetEndScale(FVector2D(FMath::Lerp(SubBranchBaseRadius,SubBranchTopRadius,a1)));
            MeshComponents.Add(M);
        }
        GenerateSubBranches(S,Depth+1);
    }
}
