// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "treeGen.generated.h"

UCLASS()
class MYPROJECT_API AtreeGen : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AtreeGen();

protected:
	virtual void OnConstruction(const FTransform& Transform) override;

	// --- Components ---
	UPROPERTY(VisibleAnywhere, Category = "Tree|Components")
	USplineComponent* TrunkSpline;

	UPROPERTY(Transient)
	TArray<USplineComponent*> BranchSplines;

	UPROPERTY(Transient)
	TArray<USplineMeshComponent*> TrunkMeshSegments;

	UPROPERTY(Transient)
	TArray<USplineMeshComponent*> BranchMeshSegments;

	FRandomStream RandomStream;

	UPROPERTY(EditAnywhere, Category = "Tree|Trunk")
	int32 Seed = 123;

	UPROPERTY(EditAnywhere, Category = "Tree|Trunk", meta=(ClampMin="2"))
	int32 NumTrunkPoints = 8;

	UPROPERTY(EditAnywhere, Category = "Tree|Trunk")
	float MinPointDistance = 50.f;

	UPROPERTY(EditAnywhere, Category = "Tree|Trunk")
	float MaxPointDistance = 150.f;

	UPROPERTY(EditAnywhere, Category = "Tree|Trunk")
	float MaxTrunkBendAngle = 20.f;

	UPROPERTY(EditAnywhere, Category = "Tree|Trunk")
	UStaticMesh* TrunkMesh;

	UPROPERTY(EditAnywhere, Category = "Tree|Trunk")
	float TrunkBaseScale = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Tree|Trunk")
	float TrunkTipScale = 0.3f;

	UPROPERTY(EditAnywhere, Category = "Tree|Branches")
	int32 NumBranches = 12;

	UPROPERTY(EditAnywhere, Category = "Tree|Branches")
	float BranchMinLength = 100.f;

	UPROPERTY(EditAnywhere, Category = "Tree|Branches")
	float BranchMaxLength = 300.f;

	UPROPERTY(EditAnywhere, Category = "Tree|Branches")
	float BranchSpawnMinHeight = 100.f;

	UPROPERTY(EditAnywhere, Category = "Tree|Branches")
	UStaticMesh* BranchMesh;

	UPROPERTY(EditAnywhere, Category = "Tree|Branches")
	float BranchBaseScale = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Tree|Branches")
	float BranchTipScale = 0.1f;

private:
	void ClearPrevious();
	void GenerateTrunk();
	void GenerateBranches();
};