// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridNode.h"
/**
 * 
 */
class TURNBASEDGRIDGAME_API FGridSystemBase
{
public:
	TArray<TArray<FGridNode>> m_grid;
	int m_gridSize = 1;
	
	virtual ~FGridSystemBase() = default;
	
	virtual void Setup(const int _size) = 0;
	virtual TArray<FIntVector2> GetNeighbourDirections() const = 0;
	virtual TArray<FGridNode> GetNeighbours(const FGridNode& _gridNode) const = 0;
	virtual TArray<FIntVector2> GetNeighbourCoords(const FIntVector2& _gridNode) const = 0;
	virtual int GetDistance(const FGridNode& _nodeA, const FGridNode& _nodeB) const = 0;
	virtual int GetDistance(const FIntVector2& _nodeA, const FIntVector2& _nodeB) const = 0;
	virtual bool TryGetGridNodeAt(FGridNode& _gridNode, const FIntVector2& _coords) const = 0;
	virtual const FVector GetRelativeLocationForNode(const FGridNode& _gridNode) const = 0;
};
