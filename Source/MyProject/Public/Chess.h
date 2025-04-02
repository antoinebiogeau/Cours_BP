// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Chess.generated.h"

UCLASS()
class MYPROJECT_API AChess : public AActor
{
	GENERATED_BODY()
	
public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ChessBoard")
	bool bRenderText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ChessBoard")
	UStaticMesh* SM_Stone;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ChessBoard")
	UMaterialInterface* Material;

	// --- Composants ---
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="ChessBoard")
	class UInstancedStaticMeshComponent* InstancedMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="ChessBoard")
	class UTextRenderComponent* InfoText;

	AChess();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
#if WITH_EDITOR
    // S'exécute lors de la construction dans l'éditeur
    virtual void OnConstruction(const FTransform& Transform) override;
#endif
};
