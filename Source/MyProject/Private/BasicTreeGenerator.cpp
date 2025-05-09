
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

    // Clear previous dynamic components
    TrunkSpline->ClearSplinePoints(false);
    for (USplineMeshComponent* Comp : SplineMeshes)
    {
        if (Comp) Comp->DestroyComponent();
    }
    SplineMeshes.Empty();
    for (USplineComponent* Comp : BranchSplines)
    {
        if (Comp) Comp->DestroyComponent();
    }
    BranchSplines.Empty();
    for (USplineComponent* Comp : SubBranchSplines)
    {
        if (Comp) Comp->DestroyComponent();
    }
    SubBranchSplines.Empty();

    if (!TrunkMesh || !BranchMesh || !SubBranchMesh)
    {
        UE_LOG(LogTemp, Warning, TEXT("Missing mesh(s) on %s"), *GetName());
        return;
    }

    FRandomStream Stream(Seed);

    // 1) Build trunk spline
    FVector Pos = FVector::ZeroVector;
    FVector Dir = FVector::UpVector;
    for (int32 i = 0; i < NBTruncPoint; ++i)
    {
        if (i > 0)
        {
            Dir = FRotator(
                Stream.FRandRange(-Angle, Angle),
                Stream.FRandRange(-Angle, Angle),
                Stream.FRandRange(-Angle, Angle)
            ).RotateVector(Dir).GetSafeNormal();
            Pos += Dir * Stream.FRandRange(MinDistPointTrunc, MaxDistPointTrunc);
        }
        TrunkSpline->AddSplinePoint(Pos, ESplineCoordinateSpace::Local, false);
    }
    TrunkSpline->UpdateSpline();

    // 2) Create trunk meshes
    for (int32 i = 0; i < NBTruncPoint - 1; ++i)
    {
        USplineMeshComponent* SM = NewObject<USplineMeshComponent>(this, *FString::Printf(TEXT("SplineMesh_T_%d"), i));
        SM->SetStaticMesh(TrunkMesh);
        SM->SetMobility(EComponentMobility::Movable);
        SM->AttachToComponent(TrunkSpline, FAttachmentTransformRules::KeepRelativeTransform);
        SM->RegisterComponentWithWorld(GetWorld());

        FVector SLoc, STan, ELoc, ETan;
        TrunkSpline->GetLocationAndTangentAtSplinePoint(i, SLoc, STan, ESplineCoordinateSpace::Local);
        TrunkSpline->GetLocationAndTangentAtSplinePoint(i + 1, ELoc, ETan, ESplineCoordinateSpace::Local);
        SM->SetStartAndEnd(SLoc, STan, ELoc, ETan);

        float A0 = float(i) / float(NBTruncPoint - 1);
        float A1 = float(i + 1) / float(NBTruncPoint - 1);
        SM->SetStartScale(FVector2D(FMath::Lerp(BaseRadius, TopRadius, A0)));
        SM->SetEndScale(FVector2D(FMath::Lerp(BaseRadius, TopRadius, A1)));
        SplineMeshes.Add(SM);
    }

    // 3) Create branches and sub-branches
    float TrLen = TrunkSpline->GetSplineLength();
    for (int32 b = 0; b < NbBranch; ++b)
    {
        // --- Branch spline setup ---
        float T0 = FMath::Lerp(BranchStart, 1.f, Stream.FRand());
        FVector BStartLoc = TrunkSpline->GetLocationAtDistanceAlongSpline(TrLen * T0, ESplineCoordinateSpace::Local);
        FVector BTan = TrunkSpline->GetTangentAtDistanceAlongSpline(TrLen * T0, ESplineCoordinateSpace::Local).GetSafeNormal();

        USplineComponent* BranchSpline = NewObject<USplineComponent>(this, *FString::Printf(TEXT("BranchSpline_%d"), b));
        BranchSpline->SetupAttachment(TrunkSpline);
        BranchSpline->RegisterComponentWithWorld(GetWorld());
        BranchSpline->ClearSplinePoints(false);
        BranchSpline->AddSplinePoint(BStartLoc, ESplineCoordinateSpace::Local, false);

        BranchSplines.Add(BranchSpline);

        // Determine initial direction
        FVector DirB = BranchDirectionBias == 0.f ? Stream.VRand().GetSafeNormal() : (BTan * BranchDirectionBias).GetSafeNormal();
        FVector CurPosB = BStartLoc;
        for (int32 p = 1; p < NbPointBranch; ++p)
        {
            DirB = FRotator(
                Stream.FRandRange(-BranchJitterAngle, BranchJitterAngle),
                Stream.FRandRange(-BranchJitterAngle, BranchJitterAngle),
                Stream.FRandRange(-BranchJitterAngle, BranchJitterAngle)
            ).RotateVector(DirB).GetSafeNormal();
            CurPosB += DirB * Stream.FRandRange(MinBranchLength, MaxBranchLength);
            BranchSpline->AddSplinePoint(CurPosB, ESplineCoordinateSpace::Local, false);
        }
        BranchSpline->UpdateSpline();

        // Branch meshes
        for (int32 i = 0; i < NbPointBranch - 1; ++i)
        {
            USplineMeshComponent* BM = NewObject<USplineMeshComponent>(this);
            BM->SetStaticMesh(BranchMesh);
            BM->SetMobility(EComponentMobility::Movable);
            BM->AttachToComponent(BranchSpline, FAttachmentTransformRules::KeepRelativeTransform);
            BM->RegisterComponentWithWorld(GetWorld());

            FVector SL, ST, EL, ET;
            BranchSpline->GetLocationAndTangentAtSplinePoint(i, SL, ST, ESplineCoordinateSpace::Local);
            BranchSpline->GetLocationAndTangentAtSplinePoint(i + 1, EL, ET, ESplineCoordinateSpace::Local);
            BM->SetStartAndEnd(SL, ST, EL, ET);

            float B0 = float(i) / float(NbPointBranch - 1);
            float B1 = float(i + 1) / float(NbPointBranch - 1);
            BM->SetStartScale(FVector2D(FMath::Lerp(BaseRadBranch, TopRadBranch, B0)));
            BM->SetEndScale(FVector2D(FMath::Lerp(BaseRadBranch, TopRadBranch, B1)));
            SplineMeshes.Add(BM);
        }

        // --- Sub-branches ---
        float BrLen = BranchSpline->GetSplineLength();
        for (int32 sb = 0; sb < NbSub; ++sb)
        {
            float tS = Stream.FRand();
            FVector SBStartLoc = BranchSpline->GetLocationAtDistanceAlongSpline(BrLen * tS, ESplineCoordinateSpace::Local);
            FVector SBDir = Stream.VRand().GetSafeNormal();

            USplineComponent* SubSpline = NewObject<USplineComponent>(this, *FString::Printf(TEXT("SubBranchSpline_%d_%d"), b, sb));
            SubSpline->SetupAttachment(BranchSpline);
            SubSpline->RegisterComponentWithWorld(GetWorld());
            SubSpline->ClearSplinePoints(false);
            SubSpline->AddSplinePoint(SBStartLoc, ESplineCoordinateSpace::Local, false);

            SubBranchSplines.Add(SubSpline);

            FVector CurPosS = SBStartLoc;
            for (int32 p2 = 1; p2 < NbPointSubBranch; ++p2)
            {
                CurPosS += SBDir * Stream.FRandRange(MinSubLength, MaxSubLength);
                SubSpline->AddSplinePoint(CurPosS, ESplineCoordinateSpace::Local, false);
            }
            SubSpline->UpdateSpline();

            // Sub-branch meshes
            for (int32 i2 = 0; i2 < NbPointSubBranch - 1; ++i2)
            {
                USplineMeshComponent* SM2 = NewObject<USplineMeshComponent>(this);
                SM2->SetStaticMesh(SubBranchMesh);
                SM2->SetMobility(EComponentMobility::Movable);
                SM2->AttachToComponent(SubSpline, FAttachmentTransformRules::KeepRelativeTransform);
                SM2->RegisterComponentWithWorld(GetWorld());

                FVector S2L, S2T, E2L, E2T;
                SubSpline->GetLocationAndTangentAtSplinePoint(i2, S2L, S2T, ESplineCoordinateSpace::Local);
                SubSpline->GetLocationAndTangentAtSplinePoint(i2 + 1, E2L, E2T, ESplineCoordinateSpace::Local);
                SM2->SetStartAndEnd(S2L, S2T, E2L, E2T);

                float S0 = float(i2) / float(NbPointSubBranch - 1);
                float S1 = float(i2 + 1) / float(NbPointSubBranch - 1);
                SM2->SetStartScale(FVector2D(FMath::Lerp(BaseSub, TopSub, S0)));
                SM2->SetEndScale(FVector2D(FMath::Lerp(BaseSub, TopSub, S1)));
                SplineMeshes.Add(SM2);
            }
        }
    }
}
