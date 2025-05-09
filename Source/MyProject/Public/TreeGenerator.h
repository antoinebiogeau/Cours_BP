// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "TreeGenerator.generated.h"
struct FColonizationNode
{
	FVector Position;
	FColonizationNode* Parent;
	TArray<FColonizationNode*> Children;
	int32 Depth;
	float OriginRatio; 
	FColonizationNode(const FVector& InPos, FColonizationNode* InParent, int32 InDepth, float InOriginRatio = 0.f)
		: Position(InPos), Parent(InParent), Depth(InDepth), OriginRatio(InOriginRatio) {}
};
UCLASS()
class MYPROJECT_API ATreeGenerator : public AActor
{
	GENERATED_BODY()

public:
	ATreeGenerator();
	virtual void OnConstruction(const FTransform& Transform) override;
	
protected:
    // ==== Trunk Settings ====
    UPROPERTY(EditAnywhere, Category = "Trunk|Shape", meta = (ToolTip = "Number of segment"))
    int32 TrunkSegments = 8;
    UPROPERTY(EditAnywhere, Category = "Trunk|Shape", meta = (ToolTip = "Min length "))
    float TrunkMinSegmentLength = 80.f;
    UPROPERTY(EditAnywhere, Category = "Trunk|Shape", meta = (ToolTip = "Max length "))
    float TrunkMaxSegmentLength = 120.f;
    UPROPERTY(EditAnywhere, Category = "Trunk|Shape", meta = (ToolTip = "Max angle jitter "))
    float TrunkJitterAngle = 10.f;
    UPROPERTY(EditAnywhere, Category = "Trunk|Branch", meta = (ToolTip = "Fraction of trunk height above which branches can spawn (0-1)"))
    float TrunkBranchStartRatio = 0.2f;
    UPROPERTY(EditAnywhere, Category = "Trunk|Branch", meta = (ToolTip = "Number of initial branch seed points along the trunk"))
    int32 BranchSeedCount = 5;
    UPROPERTY(EditAnywhere, Category = "Trunk|Thickness", meta = (ToolTip = "Base radius of trunk"))
    float TrunkBaseRadius = 20.f;
    UPROPERTY(EditAnywhere, Category = "Trunk|Thickness", meta = (ToolTip = "Top radius of trunk"))
    float TrunkTopRadius = 8.f;

    // ==== Space Colonization Settings ====
    UPROPERTY(EditAnywhere, Category = "Colonization|Attractors", meta = (ToolTip = "Number of attractor points in canopy"))
    int32 AttractorCount = 500;
    UPROPERTY(EditAnywhere, Category = "Colonization|Attractors", meta = (ToolTip = "Radius of canopy sphere"))
    float CanopyRadius = 300.f;
    UPROPERTY(EditAnywhere, Category = "Colonization|Growth", meta = (ToolTip = "Min growth length at depth=0"))
    float GrowthMinLength = 20.f;
    UPROPERTY(EditAnywhere, Category = "Colonization|Growth", meta = (ToolTip = "Max growth length at depth=0"))
    float GrowthMaxLength = 40.f;
    UPROPERTY(EditAnywhere, Category = "Colonization|Growth", meta = (ToolTip = "Growth length falloff per depth (0-1)"))
    float GrowthLengthFalloff = 0.8f;
    UPROPERTY(EditAnywhere, Category = "Colonization|Growth", meta = (ToolTip = "Min distance to consume attractor"))
    float MinAttractorDist = 30.f;
    UPROPERTY(EditAnywhere, Category = "Colonization|Growth", meta = (ToolTip = "Max influence radius of a node"))
    float MaxInfluenceDist = 100.f;
    UPROPERTY(EditAnywhere, Category = "Colonization|Growth", meta = (ToolTip = "Max colonization iterations"))
    int32 MaxColonizeIterations = 200;

    // ==== Mesh Settings ====
    UPROPERTY(EditAnywhere, Category = "Mesh", meta = (ToolTip = "Static mesh for all segments"))
    UStaticMesh* SegmentMesh = nullptr;
    UPROPERTY(EditAnywhere, Category = "Mesh", meta = (ToolTip = "Tangent scale for spline mesh curvature"))
    float TangentScale = 1.f;

    // ==== Branch Depth Settings ====
    UPROPERTY(EditAnywhere, Category = "Colonization|Depth", meta = (ToolTip = "Min depth to start branching"))
    int32 BranchStartDepth = 0;
    UPROPERTY(EditAnywhere, Category = "Colonization|Depth", meta = (ToolTip = "Max depth to grow branches"))
    int32 MaxBranchDepth = 4;

private:
    UPROPERTY(VisibleAnywhere, Transient)
    USplineComponent* TrunkSpline;

    TArray<FVector> Attractors;
    TArray<FColonizationNode*> Nodes;

    void ClearTree();
    void BuildTrunk();
    void GenerateAttractors();
    void Colonize();
    void BuildSplines();
    void CreateSplineSegment(const FVector& A, const FVector& B, float RadiusA, float RadiusB);
};

