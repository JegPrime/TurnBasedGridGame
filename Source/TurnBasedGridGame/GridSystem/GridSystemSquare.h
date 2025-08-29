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
	explicit FGridSystemSquare(const int _sizeX, const int _sizeY, const bool _doSetup = false);
	explicit FGridSystemSquare(const int _size, const bool _doSetup = false) : FGridSystemSquare(_size, _size, _doSetup) {};
	
	virtual void Setup(int _sizeX, int _sizeY);
	virtual void Setup(int _size) override;
	virtual TArray<FIntVector2> GetNeighbourDirections() const override;
	virtual TArray<FGridNode> GetNeighbourNodes(const FGridNode& _gridNode) const override;
	virtual TArray<FIntVector2> GetNeighbourCoords(const FIntVector2& _coord) const override;
	virtual int GetDistance(const FIntVector2& _coordA, const FIntVector2& _coordB) const override;
	virtual FIntVector2 GetCoordsAtLocation(const FVector& _location) const override;
	virtual FVector GetLocationAtCoords(const FIntVector2& _coord) const override;
	virtual bool IsCoordValid(const FIntVector2& _coords) const override;

	int m_gridSizeAlt = 0;
};
