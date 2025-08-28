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
	const FIntVector2 GetTileCoordsAtLocation(const FVector& _location) const;

	void GenerateGrid();
	void RemoveGrid();

private:	
	UPROPERTY(EditAnywhere, DisplayName="Tile Reference", Category="GridManager")
	TSubclassOf<AActor> m_tileMeshActorClass;
	UPROPERTY(EditAnywhere, DisplayName="Grid Size", Category="GridManager|Generation Parameters", meta=(ClampMin=1))
	int m_gridSize = 3;
	UPROPERTY(EditAnywhere, DisplayName="Tile Size", Category="GridManager|Generation Parameters", meta=(ClampMin=0.1f))
	float m_gridTileScale = 1.f;
	UPROPERTY(EditAnywhere, DisplayName="Use Single Mesh", Category="GridManager|Generation Parameters")
	bool m_useSingleMesh = true;

	const FVector m_tileDisplacement = FVector(m_gridTileScale / 2, m_gridTileScale / 2, 0);
	FGridSystemSquare* m_gridSystem = nullptr;
	TArray<TObjectPtr<AActor>> m_gridSystemActors;
};