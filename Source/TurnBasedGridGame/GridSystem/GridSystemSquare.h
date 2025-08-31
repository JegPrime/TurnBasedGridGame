// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridSystem.h"
#include "GridSystemSquare.generated.h"

UCLASS()
class TURNBASEDGRIDGAME_API UGridSystemSquare : public UGridSystem, public IGridSystem
{
	GENERATED_BODY()
public:
	void Setup(const int _sizeX, const int _sizeY);
	virtual void Setup(const int _size) override;
	virtual TArray<FIntVector2> GetNeighbourDirections() const override;
	virtual TArray<FGridNode> GetNeighbourNodes(const FGridNode& _gridNode) const override;
	virtual TArray<FIntVector2> GetNeighbourCoords(const FIntVector2& _coord) const override;
	virtual int GetDistance(const FIntVector2& _coordA, const FIntVector2& _coordB) const override;
	virtual FIntVector2 GetCoordsAtLocation(const FVector& _location) const override;
	virtual FVector GetLocationAtCoords(const FIntVector2& _coord) const override;
	virtual bool IsCoordValid(const FIntVector2& _coords) const override;
	virtual TArray<FIntVector2> GetValidCoordsWithinRange(const FIntVector2& _coords, const int _range) const override;

private:
	int m_gridSizeAlt = 0;
};
