// Fill out your copyright notice in the Description page of Project Settings.

#include "GridNode.h"
#include "GridSystemSquare.h"

TArray<FIntVector2> UGridSystemSquare::GetNeighbourDirections() const
{
	return {
			{-1,-1}, {-1,0}, {-1,1},
			{ 0,-1},         { 0,1},
			{ 1,-1}, { 1,0}, { 1,1}
	};
}

TArray<FGridNode> UGridSystemSquare::GetNeighbours(FGridNode _gridNode) const
{
	TArray<FGridNode> result;
	for (auto direction : GetNeighbourDirections())
	{
		result.Add(GetGridNodeAt(_gridNode.GetGridPosition() + direction));
	}
	return result;
}

int UGridSystemSquare::GetDistance(FGridNode _nodeStart, FGridNode _nodeEnd) const
{
	return FMath::Max(FMath::Abs(_nodeEnd.m_gridX - _nodeStart.m_gridX), FMath::Abs(_nodeEnd.m_gridY - _nodeStart.m_gridY));
}

FGridNode UGridSystemSquare::GetGridNodeAt(FIntVector2 _coords) const
{
	return m_grid[_coords.X][_coords.Y];
}