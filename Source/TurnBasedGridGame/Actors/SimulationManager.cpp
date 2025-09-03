// Fill out your copyright notice in the Description page of Project Settings.


#include "SimulationManager.h"
#include "GridManager.h"


ASimulationManager::ASimulationManager()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = m_simulationTickTime;
}

void ASimulationManager::BeginPlay()
{
	Super::BeginPlay();

	if (m_gridManager != nullptr)
	{
		if (m_gridManager->m_isGridGenerated)
		{
			Initialize();
		}
		else
		{
			m_gridManager->OnGridGenerated.AddDynamic(this, &ASimulationManager::OnGridGenerated);
		}
	}
	else
	{
		CreateGridManager();
	}
}

void ASimulationManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateSimulation();
}

void ASimulationManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (m_gridManager)
	{
		m_gridManager->OnGridGenerated.RemoveAll(this);
	}
	
	Super::EndPlay(EndPlayReason);
}

void ASimulationManager::TestMoveActor()
{
	if (TObjectPtr<AGridObject> gridObject = m_gridObjectsMap.FindOrAdd(EGridObjectTeam::Red).Last())
	{
		gridObject->Move(FVector(100,100,0), 1.f);
	}
}

void ASimulationManager::TestHitActor()
{
	if (TObjectPtr<AGridObject> gridObject = m_gridObjectsMap.FindOrAdd(EGridObjectTeam::Red).Last())
	{
		gridObject->GetHit();
	}
}

void ASimulationManager::OnGridGenerated()
{
	if (ensureMsgf(m_gridManager != nullptr, TEXT("ASimulationManager::OnGridGenerated - m_gridManager is now nullptr. This shouldn't happen")))
	{
		Initialize();
	}
}

void ASimulationManager::CreateGridManager()
{
	if (!ensureMsgf(m_gridManagerBP != nullptr, TEXT("ASimulationManager::CreateGridManager - No grid manager BP class assigned")))
	{
		return;
	}
	
	m_gridManager = Cast<AGridManager>(GetWorld()->SpawnActor(m_gridManagerBP, &FVector::ZeroVector));
	if (ensureMsgf(m_gridManager != nullptr, TEXT("ASimulationManager::CreateGridManager - m_gridManager failed initialization")))
	{
		m_gridManager->OnGridGenerated.AddDynamic(this, &ASimulationManager::OnGridGenerated);
		m_gridManager->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
	}
}

void ASimulationManager::Initialize()
{
	PrimaryActorTick.bCanEverTick = true;

	if (!ensureMsgf(m_gridManager != nullptr, TEXT("ASimulationManager::Initialize - m_gridManager failed initialization")))
	{
		return;
	}

	if (!ensureMsgf(m_gridObjectBP.Num() > 0, TEXT("ASimulationManager::Initialize - m_gridObjectBP has no entries")))
	{
		return;
	}
	
	if (!ensureMsgf(m_gridManager->m_gridSystem != nullptr, TEXT("ASimulationManager::Initialize - m_gridSystem is not set")))
	{
		return;
	}

	//Spawn Teams
	//TODO: Move actor spawning to GridManager, since we should only care about metadata here.
	for (int team = 0; team < static_cast<int>(EGridObjectTeam::Count); ++team)
	{
		const EGridObjectTeam gridTeam = static_cast<EGridObjectTeam>(team);
		if (const FIntVector2* startPos = m_startingAreaCenters.Find(gridTeam))
		{
			TArray<FIntVector2> potentialSpawnCoords = m_gridManager->m_gridSystem->GetValidCoordsWithinRange(*startPos, m_startingAreaRange);
			for (int i = 0; i < m_startingGridObjectsPerTeam && !potentialSpawnCoords.IsEmpty(); ++i)
			{
				const int randomIndex = FMath::RandRange(0, potentialSpawnCoords.Num() - 1);
				const FIntVector2 spawnCoords = potentialSpawnCoords[randomIndex];
				const TSubclassOf<AGridObject> randomGridActorBP = m_gridObjectBP[FMath::RandRange(0, m_gridObjectBP.Num() - 1)];

				if (AddSimulationObject(randomGridActorBP, spawnCoords, gridTeam))
				{
					potentialSpawnCoords.RemoveAtSwap(randomIndex);
				}
			}
		}
	}
}

void ASimulationManager::UpdateSimulation()
{
	TArray<FGridObjectSimulationData> gridObjectsToSimulate;
	m_simulationData.GenerateValueArray(gridObjectsToSimulate);
	const TArray<FGridObjectSimulationData> currentSimulationState = gridObjectsToSimulate;
	UpdateSimulationMovements(gridObjectsToSimulate, currentSimulationState);
	UpdateSimulationAttacks(gridObjectsToSimulate, currentSimulationState);
}

UE_DISABLE_OPTIMIZATION
void ASimulationManager::UpdateSimulationMovements(TArray<FGridObjectSimulationData>& simData, const TArray<FGridObjectSimulationData>& currentSimulationState)
{
	for (int i = simData.Num() - 1; i >= 0; --i)
	{
		FGridObjectSimulationData& currentSimObject = simData[i];
		FIntVector2 closestEnemyCoords = GetClosestEnemyGridDataCoords(currentSimObject, currentSimulationState);
		TArray<FIntVector2> path = GetPathToCoords(currentSimObject.m_coords, closestEnemyCoords);
		if (path.Num() > 0)
		{
			FIntVector2 newTile = path[0];
			//TODO: Remove after we have proper pathing
			if (m_simulationData.Find(newTile))
			{
				continue;
			}
			
			if (TObjectPtr<AGridObject> gridObject = m_IDtoObjectMap.FindRef(currentSimObject.m_gridID))
			{
				gridObject->Move(m_gridManager->GetTileLocation(newTile), m_simulationTickTime);
			}

			FGridObjectSimulationData newSimObject = currentSimObject;
			newSimObject.m_coords = newTile;
			m_simulationData.Remove(currentSimObject.m_coords);
			m_simulationData.Add(newTile, newSimObject);
		}
	}
}
UE_ENABLE_OPTIMIZATION

void ASimulationManager::UpdateSimulationAttacks(TArray<FGridObjectSimulationData>& simData, const TArray<FGridObjectSimulationData>& currentSimulationState)
{
	
}

bool ASimulationManager::AddSimulationObject(TSubclassOf<AGridObject> _gridObjectBP, FIntVector2 _coords, EGridObjectTeam _team)
{
	TArray<TObjectPtr<AGridObject>>& gridTeamObjectArray = m_gridObjectsMap.FindOrAdd(_team);
	const FVector spawnPointLocation = m_gridManager->GetTileLocation(_coords);
	TObjectPtr<AGridObject> spawnedGridActor = Cast<AGridObject>(GetWorld()->SpawnActor(_gridObjectBP, &spawnPointLocation));
	if (!ensureMsgf(spawnedGridActor != nullptr, TEXT("ASimulationManager::Initialize - Spawned actor is nullptr")))
	{
		return false;
	}

	GridID newID = m_uniqueGridObjectIDs++;
	spawnedGridActor->SetGridID(newID);
	gridTeamObjectArray.Add(spawnedGridActor);
	spawnedGridActor->AttachToActor(m_gridManager, FAttachmentTransformRules::KeepWorldTransform);
	spawnedGridActor->SetActorLabel(FString::Format(TEXT("GridObject_{0}_{1}"), {UEnum::GetDisplayValueAsText(_team).ToString(),newID}));
	if (UStaticMeshComponent* mesh = spawnedGridActor->GetComponentByClass<UStaticMeshComponent>())
	{
		mesh->SetCustomPrimitiveDataFloat(0, static_cast<int>(_team));
	}
	FGridObjectSimulationData simulationData;
	simulationData.m_gridID = newID;
	simulationData.m_coords = _coords;
	simulationData.m_moveSpeed = 1;
	simulationData.m_team = _team;
	m_simulationData.Add(_coords, simulationData);
	m_IDtoObjectMap.Add(simulationData.m_gridID, spawnedGridActor);
	
	return true;
}

void ASimulationManager::MoveSimulationObject()
{
	
}

void ASimulationManager::RemoveSimulationObject()
{
	
}

FIntVector2 ASimulationManager::GetClosestEnemyGridDataCoords(const FGridObjectSimulationData& _gridObjectData, const TArray<FGridObjectSimulationData>& _currentSimulationState) const
{
	if (m_gridManager == nullptr || m_gridManager->m_gridSystem == nullptr)
	{
		return FIntVector2::NoneValue;
	}
	
	int closestTileDistance = std::numeric_limits<int>::max();
	FIntVector2 closestCoords = FIntVector2::NoneValue;
	for (auto otherSimData : _currentSimulationState)
	{
		if (_gridObjectData.m_team == otherSimData.m_team)
		{
			continue;
		}

		const int tileDistance = m_gridManager->m_gridSystem->GetDistance(_gridObjectData.m_coords, otherSimData.m_coords);
		if (tileDistance < closestTileDistance)
		{
			closestTileDistance = tileDistance;
			closestCoords = otherSimData.m_coords;
		}
	}
	
	return closestCoords;
}

UE_DISABLE_OPTIMIZATION
TArray<FIntVector2> ASimulationManager::GetPathToCoords(FIntVector2 _from, FIntVector2 _to) const
{
	constexpr int MAX_PATH_LENGTH = 30; 
	
	if (m_gridManager->m_gridSystem->GetNeighbourCoords(_from).Contains(_to))
	{
		return {};
	}
	
	//BestCoordsTowardsEnemy
	int crtDist =  m_gridManager->m_gridSystem->GetDistance(_from, _to);
	for (auto coord : m_gridManager->m_gridSystem->GetNeighbourCoords(_from))
	{
		if (m_gridManager->m_gridSystem->GetDistance(coord, _to) < crtDist)
		{
			return {coord};
		}
	}
	//----------------------
	
	//A* here
	return {};
}
UE_ENABLE_OPTIMIZATION

