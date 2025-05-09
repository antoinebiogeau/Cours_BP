// ProceduralTreeGenerator.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "ProceduralTreeGenerator.generated.h"

/**
 * Node for space colonization stored in a preallocated pool to avoid dynamic allocations.
 */
USTRUCT()
struct FColonizationNode
{
    GENERATED_BODY()
    // Local position of this node
    FVector Position;
    // Index into NodePool of parent node (-1 for trunk seeds)
    int32 ParentIndex;
    // Depth level of branch
    int32 Depth;
    // Fraction along trunk where this branch seed originated (0=base,1=top)
    float OriginRatio;
};

UCLASS()
class MYPROJECT_API AProceduralTreeGenerator : public AActor
{
    GENERATED_BODY()

public:
    AProceduralTreeGenerator();
    virtual void OnConstruction(const FTransform& Transform) override;
#if WITH_EDITOR
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

protected:
    /** Only rebuild when parameters change */
    bool bNeedsRebuild = true;

    // === Trunk Settings ===
    UPROPERTY(EditAnywhere, Category = "Trunk|Shape")
    int32 TrunkSegments = 8;
    UPROPERTY(EditAnywhere, Category = "Trunk|Shape")
    float TrunkMinSegmentLength = 80.f;
    UPROPERTY(EditAnywhere, Category = "Trunk|Shape")
    float TrunkMaxSegmentLength = 120.f;
    UPROPERTY(EditAnywhere, Category = "Trunk|Shape")
    float TrunkJitterAngle = 10.f;
    UPROPERTY(EditAnywhere, Category = "Trunk|Branch")
    float TrunkBranchStartRatio = 0.2f;
    UPROPERTY(EditAnywhere, Category = "Trunk|Branch")
    int32 BranchSeedCount = 5;
    UPROPERTY(EditAnywhere, Category = "Trunk|Thickness")
    float TrunkBaseRadius = 20.f;
    UPROPERTY(EditAnywhere, Category = "Trunk|Thickness")
    float TrunkTopRadius = 8.f;

    // === Colonization Settings ===
    UPROPERTY(EditAnywhere, Category = "Colonization|Attractors")
    int32 AttractorCount = 500;
    UPROPERTY(EditAnywhere, Category = "Colonization|Attractors")
    float CanopyRadius = 300.f;
    UPROPERTY(EditAnywhere, Category = "Colonization|Growth")
    float GrowthMinLength = 20.f;
    UPROPERTY(EditAnywhere, Category = "Colonization|Growth")
    float GrowthMaxLength = 40.f;
    UPROPERTY(EditAnywhere, Category = "Colonization|Growth")
    float GrowthLengthFalloff = 0.8f;
    UPROPERTY(EditAnywhere, Category = "Colonization|Growth")
    float MinAttractorDist = 30.f;
    UPROPERTY(EditAnywhere, Category = "Colonization|Growth")
    float MaxInfluenceDist = 100.f;
    UPROPERTY(EditAnywhere, Category = "Colonization|Growth")
    int32 MaxColonizeIterations = 200;
    UPROPERTY(EditAnywhere, Category = "Colonization|Depth")
    int32 BranchStartDepth = 0;
    UPROPERTY(EditAnywhere, Category = "Colonization|Depth")
    int32 MaxBranchDepth = 4;

    // === Mesh Settings (Instanced) ===
    UPROPERTY(VisibleAnywhere, Category = "Mesh")
    UHierarchicalInstancedStaticMeshComponent* InstancedMesh;
    UPROPERTY(EditAnywhere, Category = "Mesh")
    UStaticMesh* SegmentMesh = nullptr;
    UPROPERTY(EditAnywhere, Category = "Mesh")
    float TangentScale = 1.f;
    UPROPERTY(EditAnywhere, Category = "Mesh|Branches")
    float BranchBaseRadiusMin = 5.f;
    UPROPERTY(EditAnywhere, Category = "Mesh|Branches")
    float BranchBaseRadiusMax = 15.f;

private:
    // Spline for the trunk
    UPROPERTY(VisibleAnywhere)
    USplineComponent* TrunkSpline;

    // Pools for nodes and attractors
    TArray<FColonizationNode> NodePool;
    TArray<FVector> Attractors;

    // Generation steps
    void ClearTree();
    void BuildTrunk();
    void GenerateAttractors();
    void Colonize();
    void BuildInstancedMeshes();
};
