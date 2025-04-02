#include "CubesInSphere.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Engine/Engine.h"
#include "Math/UnrealMathUtility.h"
#include "Kismet/KismetMathLibrary.h"

ACubesInSphere::ACubesInSphere()
{
	PrimaryActorTick.bCanEverTick = false;

	// Création et attachement des composants
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(RootComponent);

	InstancedMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("InstancedMesh"));
	InstancedMesh->SetupAttachment(RootComponent);

	InfoText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("InfoText"));
	InfoText->SetupAttachment(RootComponent);

	// Valeurs par défaut
	Cubes_NUM = 10;
	Radius = 300.f;
	ErrorTolerance = 0.5f; // Par exemple : 0.5 * (taille du BoxExtent)
	Outer_Scale = FVector(0.5f);           // Scale pour les cubes proches du rayon
	Inner_Scale = FVector(1.f, 0.1f, 0.1f);  // Scale pour les cubes intérieurs
	bRenderText = true;
	SM_Stone = nullptr;
	Material = nullptr;
}

void ACubesInSphere::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	// Vérifications préliminaires
	if (Cubes_NUM < 1 || !SM_Stone || !Material)
	{
		InstancedMesh->ClearInstances();
		return;
	}

	// Optionnel : vérifier que le mesh est "Sm_ChamferCube"
	if (!SM_Stone->GetName().Equals(TEXT("Sm_ChamferCube"), ESearchCase::IgnoreCase))
	{
		// return; // ou forcer la valeur
	}

	InstancedMesh->ClearInstances();
	InstancedMesh->SetStaticMesh(SM_Stone);
	InstancedMesh->SetMaterial(0, Material);
	InstancedMesh->NumCustomDataFloats = 3;

	// Récupération de la bounding box du mesh
	const FBoxSphereBounds MeshBounds = SM_Stone->GetBounds();
	// Utiliser la taille (norme du BoxExtent) pour définir une tolérance en unités
	const float AdjustedTolerance = ErrorTolerance * MeshBounds.BoxExtent.Size();

	// Génération des instances
	for (int32 i = 0; i < Cubes_NUM; ++i)
	{
		// 1) Générer une position aléatoire dans la sphère de rayon "Radius"
		FVector Pos;
		while (true)
		{
			const float x = FMath::FRandRange(-1.f, 1.f);
			const float y = FMath::FRandRange(-1.f, 1.f);
			const float z = FMath::FRandRange(-1.f, 1.f);
			if (x * x + y * y + z * z <= 1.f)
			{
				const FVector Dir = FVector(x, y, z).GetSafeNormal();
				const float Mag = FMath::FRandRange(0.f, 1.f);
				Pos = Dir * (Mag * Radius);
				break;
			}
		}

		// 2) Calculer la distance au centre
		const float Dist = Pos.Size();
		// Si la distance au rayon est inférieure ou égale à la tolérance ajustée, c'est un "outer cube"
		const bool bIsOuter = (FMath::Abs(Dist - Radius) <= AdjustedTolerance);
		const FVector Scale = bIsOuter ? Outer_Scale : Inner_Scale;

		// 3) Calculer une rotation "look at" vers le centre
		const FRotator Rot = UKismetMathLibrary::FindLookAtRotation(Pos, FVector::ZeroVector);

		// 4) Construire la transformation
		FTransform InstanceTransform;
		InstanceTransform.SetLocation(Pos);
		InstanceTransform.SetRotation(Rot.Quaternion());
		InstanceTransform.SetScale3D(Scale);

		// Ajouter l'instance et assigner une couleur aléatoire
		const int32 Idx = InstancedMesh->AddInstance(InstanceTransform);
		const float RVal = FMath::FRand();
		const float GVal = FMath::FRand();
		const float BVal = FMath::FRand();
		InstancedMesh->SetCustomDataValue(Idx, 0, RVal, false);
		InstancedMesh->SetCustomDataValue(Idx, 1, GVal, false);
		InstancedMesh->SetCustomDataValue(Idx, 2, BVal, true);
	}

	// 5) Mettre à jour le texte d'information
	const FString InfoString = FString::Printf(TEXT("Cube Count: %d\nBlueprint Name: %s"), Cubes_NUM, *GetFName().ToString());
	InfoText->SetText(FText::FromString(InfoString));
	InfoText->SetVisibility(bRenderText);
}

void ACubesInSphere::BeginPlay()
{
	Super::BeginPlay();
}

void ACubesInSphere::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
