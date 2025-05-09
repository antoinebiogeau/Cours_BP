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
    UPROPERTY(VisibleAnywhere, Category = "Tree|Components")
    USceneComponent* RootComp;

    UPROPERTY(VisibleAnywhere, Category = "Tree|Components")
    USplineComponent* TrunkSpline;

    UPROPERTY()
    TArray<USplineMeshComponent*> SplineMeshes;

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
};
