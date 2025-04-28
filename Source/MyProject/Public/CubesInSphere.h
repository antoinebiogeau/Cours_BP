#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CubesInSphere.generated.h"

UCLASS()
class MYPROJECT_API ACubesInSphere : public AActor
{
	GENERATED_BODY()

public:
	ACubesInSphere();
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sphere Settings")
	int32 Cubes_NUM;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sphere Settings")
	float Radius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sphere Settings")
	float ErrorTolerance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sphere Settings")
	FVector Outer_Scale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sphere Settings")
	FVector Inner_Scale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sphere Settings")
	bool bRenderText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sphere Settings")
	UStaticMesh* SM_Stone;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sphere Settings")
	UMaterialInterface* Material;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UInstancedStaticMeshComponent* InstancedMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UTextRenderComponent* InfoText;
};
