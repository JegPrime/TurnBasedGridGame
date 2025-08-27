// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GridSystemBase.generated.h"

struct FGridNode;

/**
 * 
 */
UCLASS(Abstract)
class TURNBASEDGRIDGAME_API UGridSystemBase : public UObject
{
	GENERATED_BODY()
public:
	TArray<TArray<FGridNode>> m_grid; 
	
	virtual TArray<FIntVector2> GetNeighbourDirections() const;
	virtual TArray<FGridNode> GetNeighbours(FGridNode _gridNode) const;
	virtual int GetDistance(FGridNode _nodeA, FGridNode _nodeB) const;
	virtual FGridNode GetGridNodeAt(FIntVector2 _coords) const;
};
