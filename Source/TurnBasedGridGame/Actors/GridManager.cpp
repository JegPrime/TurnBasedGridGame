// Fill out your copyright notice in the Description page of Project Settings.


#include "GridManager.h"

#include "Kismet/GameplayStatics.h"


// Sets default values
AGridManager::AGridManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AGridManager::BeginPlay()
{
	Super::BeginPlay();

	ensureMsgf(m_tileMeshActorClass != nullptr, TEXT("AGridManager::AGridManager - m_tileMesh is nullptr"));
	ensureMsgf(ensure(m_gridSize > 1), TEXT("AGridManager::AGridManager - m_gridSize too small"));

	m_gridSystem = NewObject<UGridSystemSquare>();
	m_gridSystem->Setup(m_gridSize);
	
	for (auto gridLine : m_gridSystem->m_grid)
	{
		for (auto gridNode : gridLine)
		{
			const FVector tileLocation = GetTileLocation(gridNode);
			GetWorld()->SpawnActor(m_tileMeshActorClass, &tileLocation);
		}
	}
}

void AGridManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

const FVector AGridManager::GetTileLocation(const FGridNode& _gridNode) const
{
	FVector tileLocation = m_gridSystem->GetRelativeLocationForNode(_gridNode);
	tileLocation *= m_gridTileSize * m_gridTileScale;
	return tileLocation;
}

