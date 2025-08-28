// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridSystemBase.h"

/**
 * 
 */
class TURNBASEDGRIDGAME_API FGridSystemSquare : public FGridSystemBase
{
public:
	explicit FGridSystemSquare(const int _size);
	explicit FGridSystemSquare(const int _sizeX, const int _sizeY);
	
	virtual void Setup(int _sizeX, int _sizeY);
	virtual void Setup(int _size) override;
	virtual TArray<FIntVector2> GetNeighbourDirections() const override;
	virtual TArray<FGridNode> GetNeighbours(const FGridNode& _gridNode) const override;
	virtual int GetDistance(const FGridNode& _nodeA, const FGridNode& _nodeB) const override;
	virtual bool TryGetGridNodeAt(FGridNode& _gridNode, const FIntVector2& _coords) const override;
	virtual const FVector GetRelativeLocationForNode(const FGridNode& _gridNode) const override;
};
