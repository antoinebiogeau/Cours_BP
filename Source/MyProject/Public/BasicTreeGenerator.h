// BasicTreeGenerator.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "BasicTreeGenerator.generated.h"

UCLASS()
class MYPROJECT_API ABasicTreeGenerator : public AActor
{
    GENERATED_BODY()

public:
    ABasicTreeGenerator();
    virtual void OnConstruction(const FTransform& Transform) override;

protected:
    // Root component and trunk spline
    UPROPERTY(VisibleAnywhere, Category = "Tree|Components")
    USceneComponent* RootComp;

    UPROPERTY(VisibleAnywhere, Category = "Tree|Components")
    USplineComponent* TrunkSpline;

    // Keep track of dynamic components to clear
    UPROPERTY()
    TArray<USplineMeshComponent*> SplineMeshes;
    UPROPERTY()
    TArray<USplineComponent*> BranchSplines;
    UPROPERTY()
    TArray<USplineComponent*> SubBranchSplines;

    // Trunk parameters
    UPROPERTY(EditAnywhere, Category = "Tree|Trunk")
    int32 Seed = 521;

    UPROPERTY(EditAnywhere, Category = "Tree|Trunk", meta = (ClampMin = "2", UIMin = "2"))
    int32 NBTruncPoint = 20;

    UPROPERTY(EditAnywhere, Category = "Tree|Trunk")
    float MinDistPointTrunc = 100.f;

    UPROPERTY(EditAnywhere, Category = "Tree|Trunk")
    float MaxDistPointTrunc = 200.f;

    UPROPERTY(EditAnywhere, Category = "Tree|Trunk")
    float Angle = 10.f;

    UPROPERTY(EditAnywhere, Category = "Tree|Trunk")
    float BaseRadius = 1.5f;

    UPROPERTY(EditAnywhere, Category = "Tree|Trunk")
    float TopRadius = 0.5f;

    UPROPERTY(EditAnywhere, Category = "Tree|Trunk")
    UStaticMesh* TrunkMesh;

    // Branch parameters
    UPROPERTY(EditAnywhere, Category = "Tree|Branch", meta = (ClampMin = "0", ClampMax = "1"))
    float BranchStart = 0.2f;

    UPROPERTY(EditAnywhere, Category = "Tree|Branch", meta = (ClampMin = "1", UIMin = "1"))
    int32 NbBranch = 50;

    UPROPERTY(EditAnywhere, Category = "Tree|Branch", meta = (ClampMin = "2", UIMin = "2"))
    int32 NbPointBranch = 6;

    UPROPERTY(EditAnywhere, Category = "Tree|Branch")
    float MinBranchLength = 300.f;

    UPROPERTY(EditAnywhere, Category = "Tree|Branch")
    float MaxBranchLength = 500.f;

    UPROPERTY(EditAnywhere, Category = "Tree|Branch")
    float BranchJitterAngle = 40.f;

    UPROPERTY(EditAnywhere, Category = "Tree|Branch")
    float BaseRadBranch = 0.1f;

    UPROPERTY(EditAnywhere, Category = "Tree|Branch")
    float TopRadBranch = 0.8f;

    UPROPERTY(EditAnywhere, Category = "Tree|Branch")
    UStaticMesh* BranchMesh;

    UPROPERTY(EditAnywhere, Category = "Tree|Branch", meta = (ClampMin = "-1", ClampMax = "1"))
    float BranchDirectionBias = 0.f;

    // Sub-branch parameters
    UPROPERTY(EditAnywhere, Category = "Tree|SubBranch", meta = (ClampMin = "2", UIMin = "2"))
    int32 NbPointSubBranch = 4;

    UPROPERTY(EditAnywhere, Category = "Tree|SubBranch")
    float MinSubLength = 100.f;

    UPROPERTY(EditAnywhere, Category = "Tree|SubBranch")
    float MaxSubLength = 200.f;

    UPROPERTY(EditAnywhere, Category = "Tree|SubBranch", meta = (ClampMin = "1", UIMin = "1"))
    int32 NbSub = 10;

    UPROPERTY(EditAnywhere, Category = "Tree|SubBranch")
    float BaseSub = 0.3f;

    UPROPERTY(EditAnywhere, Category = "Tree|SubBranch")
    float TopSub = 0.001f;

    UPROPERTY(EditAnywhere, Category = "Tree|SubBranch")
    UStaticMesh* SubBranchMesh;
};