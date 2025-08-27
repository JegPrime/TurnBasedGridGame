// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridNode.generated.h"

USTRUCT()
struct FGridData
{
	GENERATED_BODY()
};

/**
 * 
 */
USTRUCT()
struct FGridNode
{
	GENERATED_BODY()
	int m_gridX = 0;
	int m_gridY = 0;

	TArray<FGridData> m_gridData;

	FGridNode()
	{
		m_gridX = 0;
		m_gridY = 0;
	}
	
	FGridNode(int _gridX, int _gridY)
	{
		m_gridX = _gridX;
		m_gridY = _gridY;
	}

	const FIntVector2 GetGridPosition() const { return FIntVector2(m_gridX, m_gridY); }
};
