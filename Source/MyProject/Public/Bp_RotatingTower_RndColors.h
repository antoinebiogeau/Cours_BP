// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bp_RotatingTower_RndColors.generated.h"

UCLASS()
class MYPROJECT_API ABp_RotatingTower_RndColors : public AActor
{
	GENERATED_BODY()
public:
	ABp_RotatingTower_RndColors();
#if WITH_EDITOR
	virtual void OnConstruction(const FTransform& T) override;
#endif
	virtual void BeginPlay() override;
	virtual void Tick(float D) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TS")
	int32 Stones_Z;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TS")
	bool bRenderText;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TS")
	UStaticMesh* SM_Stone;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TS")
	UMaterialInterface* Material;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="C")
	class UInstancedStaticMeshComponent* ISM;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="C")
	class UTextRenderComponent* TXT;
};
