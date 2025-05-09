// BasicTreeGenerator.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "BasicTreeGenerator.generated.h"

UCLASS()
class MYPROJECT_API ABasicTreeGenerator : public AActor
{
    GENERATED_BODY()

public:
    ABasicTreeGenerator();

#if WITH_EDITOR
    virtual void OnConstruction(const FTransform& Transform) override;
#endif

protected:
    // Root and trunk spline
    UPROPERTY(VisibleAnywhere, Category="Tree")
    USceneComponent* Root;

    UPROPERTY(VisibleAnywhere, Category="Tree")
    USplineComponent* TrunkSpline;

    // Trunk parameters
    UPROPERTY(EditAnywhere, Category="Tree|Trunk") int32 TrunkPointCount = 20;
    UPROPERTY(EditAnywhere, Category="Tree|Trunk") float MinTrunkSegmentLength = 100.f;
    UPROPERTY(EditAnywhere, Category="Tree|Trunk") float MaxTrunkSegmentLength = 200.f;
    UPROPERTY(EditAnywhere, Category="Tree|Trunk") float TrunkJitterAngle = 10.f;

    // Branch parameters
    UPROPERTY(EditAnywhere, Category="Tree|Branch") float BranchStartRatio = 0.2f;
    UPROPERTY(EditAnywhere, Category="Tree|Branch") int32 BranchCount = 50;
    UPROPERTY(EditAnywhere, Category="Tree|Branch") int32 BranchPointCount = 6;
    UPROPERTY(EditAnywhere, Category="Tree|Branch") float MinBranchLength = 300.f;
    UPROPERTY(EditAnywhere, Category="Tree|Branch") float MaxBranchLength = 500.f;
    UPROPERTY(EditAnywhere, Category="Tree|Branch") float BranchJitterAngle = 40.f;
    UPROPERTY(EditAnywhere, Category="Tree|Branch") float BranchBaseRadius = 0.1f;
    UPROPERTY(EditAnywhere, Category="Tree|Branch") float BranchTopRadius = 0.8f;
    UPROPERTY(EditAnywhere, Category="Tree|Branch") float BranchTangentInfluence = 1.f;

    // Sub-branch parameters
    UPROPERTY(EditAnywhere, Category="Tree|SubBranch") int32 SubBranchCount = 20;
    UPROPERTY(EditAnywhere, Category="Tree|SubBranch") int32 SubBranchPointCount = 4;
    UPROPERTY(EditAnywhere, Category="Tree|SubBranch") float MinSubBranchLength = 150.f;
    UPROPERTY(EditAnywhere, Category="Tree|SubBranch") float MaxSubBranchLength = 300.f;
    UPROPERTY(EditAnywhere, Category="Tree|SubBranch") float SubBranchJitterAngle = 30.f;
    UPROPERTY(EditAnywhere, Category="Tree|SubBranch") float SubBranchBaseRadius = 0.05f;
    UPROPERTY(EditAnywhere, Category="Tree|SubBranch") float SubBranchTopRadius = 0.3f;

    // Mesh settings
    UPROPERTY(EditAnywhere, Category="Tree|Mesh") UStaticMesh* SegmentMesh;
    UPROPERTY(EditAnywhere, Category="Tree|Mesh") float TangentScale = 1.f;

private:
    // Runtime containers
    TArray<USplineMeshComponent*> MeshComponents;
    TArray<USplineComponent*> BranchSplines;

    // Generation methods
    void ClearAllComponents();
    void BuildTrunk();
    void BuildBranches();
    void GenerateSubBranches(USplineComponent* ParentSpline, int32 Depth = 0);
};