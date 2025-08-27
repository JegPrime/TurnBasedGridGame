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

TArray<FGridNode> UGridSystemSquare::GetNeighbours(const FGridNode& _gridNode) const
{
	TArray<FGridNode> result;
	for (auto direction : GetNeighbourDirections())
	{
		FGridNode gridNode;
		if (TryGetGridNodeAt(gridNode, _gridNode.GetGridPosition() + direction))
		{
			result.Add(gridNode);	
		}
	}
	return result;
}

int UGridSystemSquare::GetDistance(const FGridNode& _nodeStart, const FGridNode& _nodeEnd) const
{
	return FMath::Max(FMath::Abs(_nodeEnd.m_gridX - _nodeStart.m_gridX), FMath::Abs(_nodeEnd.m_gridY - _nodeStart.m_gridY));
}

bool UGridSystemSquare::TryGetGridNodeAt(FGridNode&_gridNode, const FIntVector2& _coords) const
{
	if (!FMath::IsWithin(_coords.X, 0, m_gridSize) ||
		!FMath::IsWithin(_coords.Y, 0, m_gridSize))
	{
		return false;
	}
	
	_gridNode = m_grid[_coords.X][_coords.Y];
	return true;
}