// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridNode.h"
#include "GridSystem.generated.h"

UINTERFACE()
class UGridSystem : public UInterface
{
	GENERATED_BODY()
};

class TURNBASEDGRIDGAME_API IGridSystem
{
	GENERATED_BODY()
public:	
	virtual void Setup(const int _size) = 0;
	virtual TArray<FIntVector2> GetNeighbourDirections() const = 0;
	virtual TArray<FGridNode> GetNeighbourNodes(const FGridNode& _gridNode) const = 0;
	virtual TArray<FIntVector2> GetNeighbourCoords(const FGridNode& _gridNode) const { return GetNeighbourCoords(_gridNode.GetGridPosition()); }
	virtual TArray<FIntVector2> GetNeighbourCoords(const FIntVector2& _gridNode) const = 0;
	virtual int GetDistance(const FGridNode& _nodeA, const FGridNode& _nodeB) const { return GetDistance(_nodeA.GetGridPosition(), _nodeB.GetGridPosition()); }
	virtual int GetDistance(const FIntVector2& _coordA, const FIntVector2& _coordB) const = 0;
	virtual FIntVector2 GetCoordsAtLocation(const FVector& _location) const = 0;
	virtual FVector GetLocationAtCoords(const FIntVector2& _coord) const = 0;
	virtual FVector GetLocationAtNode(const FGridNode& _gridNode) const { return GetLocationAtCoords(_gridNode.GetGridPosition()); }
	virtual bool IsCoordValid(const FIntVector2& _coords) const = 0;
	virtual TArray<FIntVector2> GetValidCoordsWithinRange(const FIntVector2& _coords, const int _range) const = 0;

	TArray<TArray<FGridNode>> m_grid;
	int m_gridSize = 0;
};