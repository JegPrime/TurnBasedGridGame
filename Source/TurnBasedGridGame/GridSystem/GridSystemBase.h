// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridNode.h"
#include "UObject/Object.h"
#include "GridSystemBase.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class TURNBASEDGRIDGAME_API UGridSystemBase : public UObject
{
	GENERATED_BODY()
public:
	TArray<TArray<FGridNode>> m_grid;
	int m_gridSize = 0;
	
	virtual void Setup(int _size) {}
	virtual TArray<FIntVector2> GetNeighbourDirections() const { return TArray<FIntVector2>(); };
	virtual TArray<FGridNode> GetNeighbours(const FGridNode& _gridNode) const { return TArray<FGridNode>(); }
	virtual int GetDistance(const FGridNode& _nodeA, const FGridNode& _nodeB) const { return 0; }
	virtual bool TryGetGridNodeAt(FGridNode& _gridNode, const FIntVector2& _coords) const { return false; }
	virtual const FVector GetRelativeLocationForNode(const FGridNode& _gridNode) const { return FVector::ZeroVector; }
};
