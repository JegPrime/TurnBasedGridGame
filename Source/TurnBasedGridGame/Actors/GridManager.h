// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TurnBasedGridGame/GridSystem/GridSystemSquare.h"
#include "GridManager.generated.h"

UCLASS()
class TURNBASEDGRIDGAME_API AGridManager : public AActor
{
	GENERATED_BODY()

public:
	AGridManager();

private:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaTime) override;

	const FVector GetTileLocation(const FGridNode& _gridNode) const;

private:
	UPROPERTY(EditAnywhere)
	UClass* m_tileMeshActorClass;
	UPROPERTY(EditAnywhere, meta=(ClampMin=1))
	int m_gridSize = 3;
	UPROPERTY(EditAnywhere, meta=(ClampMin=0.1f))
	float m_gridTileScale = 1.f;
	UPROPERTY(EditAnywhere)
	bool m_useSingleMesh = true;
	
	FGridSystemSquare* m_gridSystem;
};