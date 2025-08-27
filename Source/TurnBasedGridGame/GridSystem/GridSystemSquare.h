// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridSystemBase.h"
#include "UObject/Object.h"
#include "GridSystemSquare.generated.h"

struct FGridNode;

/**
 * 
 */
UCLASS()
class TURNBASEDGRIDGAME_API UGridSystemSquare : public UGridSystemBase
{
	GENERATED_BODY()
	
public:
	virtual TArray<FIntVector2> GetNeighbourDirections() const override;
	virtual TArray<FGridNode> GetNeighbours(FGridNode _gridNode) const override;
	virtual int GetDistance(FGridNode _nodeA, FGridNode _nodeB) const override;
	virtual FGridNode GetGridNodeAt(FIntVector2 _coords) const override;
};
