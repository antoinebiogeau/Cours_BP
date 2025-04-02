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

	// Appelé dans l'éditeur lors d'un changement
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// Paramètres éditables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sphere Settings")
	int32 Cubes_NUM;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sphere Settings")
	float Radius;

	// Facteur relatif pour calculer la tolérance en fonction de la taille du mesh
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sphere Settings")
	float ErrorTolerance;

	// Scale appliquée aux cubes proches du rayon (outer)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sphere Settings")
	FVector Outer_Scale;

	// Scale appliquée aux cubes à l’intérieur (inner)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sphere Settings")
	FVector Inner_Scale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sphere Settings")
	bool bRenderText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sphere Settings")
	UStaticMesh* SM_Stone;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sphere Settings")
	UMaterialInterface* Material;

	// Composants
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UInstancedStaticMeshComponent* InstancedMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UTextRenderComponent* InfoText;
};
