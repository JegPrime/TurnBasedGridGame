// Fill out your copyright notice in the Description page of Project Settings.

#include "GridSystemSquare.h"
#include "GridNode.h"

void UGridSystemSquare::Setup(int _sizeX, int _sizeY)
{
	for (int i = 0; i < _sizeX; ++i)
	{
		TArray<FGridNode> gridNodeLine;
		for (int j = 0; j < _sizeY; ++j)
		{
			gridNodeLine.Add(FGridNode(i,j));
		}
		m_grid.Add(gridNodeLine);
	}

	m_gridSize = _sizeX;
	m_gridSizeAlt = _sizeY;
}

void UGridSystemSquare::Setup(int _sizeX)
{
	Setup(_sizeX, _sizeX);
}

TArray<FIntVector2> UGridSystemSquare::GetNeighbourDirections() const
{
	return
	{
			{-1,-1}, {-1,0}, {-1,1},
			{ 0,-1},         { 0,1},
			{ 1,-1}, { 1,0}, { 1,1}
	};
}

TArray<FGridNode> UGridSystemSquare::GetNeighbourNodes(const FGridNode& _gridNode) const
{
	TArray<FGridNode> result;
	for (auto direction : GetNeighbourDirections())
	{
		const FIntVector2 neighbourGridCoord = _gridNode.GetGridPosition() + direction; 
		if (IsCoordValid(neighbourGridCoord))
		{
			result.Add(m_grid[neighbourGridCoord.X][neighbourGridCoord.Y]);
		}
	}
	return result;
}

TArray<FIntVector2> UGridSystemSquare::GetNeighbourCoords(const FIntVector2& _coord) const
{
	TArray<FIntVector2> result;
	for (auto direction : GetNeighbourDirections())
	{
		if (IsCoordValid(_coord))
		{
			result.Add(_coord + direction);
		}
	}
	return result;
}

int UGridSystemSquare::GetDistance(const FIntVector2& _coordA, const FIntVector2& _coordB) const
{
	return FMath::Max(FMath::Abs(_coordB.X - _coordA.X), FMath::Abs(_coordB.Y - _coordA.Y));
}

FIntVector2 UGridSystemSquare::GetCoordsAtLocation(const FVector& _location) const
{
	return FIntVector2(FMath::Floor(_location.X), FMath::Floor(_location.Y));
}

FVector UGridSystemSquare::GetLocationAtCoords(const FIntVector2& _coord) const
{
	return FVector(_coord.X, _coord.Y, 0.0f);
}

bool UGridSystemSquare::IsCoordValid(const FIntVector2& _coords) const
{
	if (m_gridSize == 0 || m_gridSizeAlt == 0)
	{
		return false;
	}
	
	if (!FMath::IsWithin(_coords.X, 1, m_gridSize) ||
		!FMath::IsWithin(_coords.Y, 1, m_gridSizeAlt))
	{
		return false;
	}

	return true;
}
