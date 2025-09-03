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
	m_simulationGridState.Empty();
	for (auto data : m_simulationStateData)
	{
		m_simulationGridState.Add(data.m_coords, data);
	}

	UpdateSimulationMovements(m_simulationStateData, m_simulationStateData);
	UpdateSimulationAttacks(m_simulationStateData, m_simulationStateData);

	ReadSimulationData();
}

UE_DISABLE_OPTIMIZATION
void ASimulationManager::UpdateSimulationMovements(TArray<FGridObjectSimulationData>& _simulationData, const TArray<FGridObjectSimulationData>& _simulationState)
{
	m_simulationMoveData.Empty();
	for (int i = _simulationData.Num() - 1; i >= 0; --i)
	{
		FGridObjectSimulationData& currentSimObject = _simulationData[i];
		currentSimObject.m_hasMoved = false;
		FIntVector2 closestEnemyCoords = GetClosestEnemyGridDataCoords(currentSimObject, _simulationState);
		TArray<FIntVector2> path = GetPathToCoords(currentSimObject.m_coords, closestEnemyCoords);
		if (path.Num() > 0 && currentSimObject.m_moveSpeed > 0)
		{
			const int tilesMoved = FMath::Min(path.Num(), currentSimObject.m_moveSpeed) - 1;
			FIntVector2 newTile = path[tilesMoved];
			
			//TODO: Remove after we have proper pathing. We're checking for other objects here.
			if (m_simulationGridState.Find(newTile))
			{
				continue;
			}

			//TODO: Add all the moved tiles as checkpoints for final movement
			m_simulationMoveData.Add(FSimMoveData(currentSimObject.m_gridID, newTile));
			currentSimObject.m_hasMoved = true;

			m_simulationGridState.Remove(currentSimObject.m_coords);
			currentSimObject.m_coords = newTile;
			m_simulationGridState.Add(newTile, currentSimObject);
		}
	}
}


void ASimulationManager::UpdateSimulationAttacks(TArray<FGridObjectSimulationData>& _simData, const TArray<FGridObjectSimulationData>& _currentSimulationState)
{
	m_simulationAttackData.Empty();
	m_simulationDeathData.Empty();
	for (auto simData : _currentSimulationState)
	{
		if (simData.m_hasMoved)
		{
			continue;
		}

		for (FIntVector2 neighbourCoords : m_gridManager->m_gridSystem->GetNeighbourCoords(simData.m_coords))
		{
			if (FGridObjectSimulationData* neighbourSimData = m_simulationGridState.Find(neighbourCoords))
			{
				if (!neighbourSimData->m_hasMoved && simData.m_team != neighbourSimData->m_team)
				{
					m_simulationAttackData.Add(FSimAttackData(simData.m_gridID, neighbourSimData->m_gridID));

					if (neighbourSimData->m_health <= simData.m_damage)
					{
						neighbourSimData->m_health = 0;
						m_simulationDeathData.AddUnique(FSimDeathData(neighbourSimData->m_gridID));
					}
					else
					{
						neighbourSimData->m_health -= simData.m_damage;
					}
				}
			}	
		}
	}

	for (auto [deathID] : m_simulationDeathData)
	{
		for (int i = m_simulationStateData.Num() - 1; i >= 0; --i)
		{
			if (deathID == m_simulationStateData[i].m_gridID)
			{
				m_simulationStateData.RemoveAtSwap(i);
				break;
			}
		}
	}
}
UE_ENABLE_OPTIMIZATION

void ASimulationManager::ReadSimulationData()
{
	for (auto [gridID, coords] : m_simulationMoveData)
	{
		if (TObjectPtr<AGridObject> gridObject = m_IDtoObjectMap.FindRef(gridID))
		{
			gridObject->Move(m_gridManager->GetTileLocation(coords), m_simulationTickTime);
		}
	}

	for (auto [attackerID, targetID] : m_simulationAttackData)
	{
		TObjectPtr<AGridObject> attacker = m_IDtoObjectMap.FindRef(attackerID);
		TObjectPtr<AGridObject> target = m_IDtoObjectMap.FindRef(targetID);
		if (attacker != nullptr && target != nullptr)
		{
			target->GetHit();
			// attacker attack
			// target get hit
		}
	}

	for (auto [deathID] : m_simulationDeathData)
	{
		RemoveSimulationObject(deathID);
	}
}

bool ASimulationManager::AddSimulationObject(TSubclassOf<AGridObject> _gridObjectBP, FIntVector2 _coords, EGridObjectTeam _team)
{
	TArray<TObjectPtr<AGridObject>>& gridTeamObjectArray = m_gridObjectsMap.FindOrAdd(_team);
	const FVector spawnPointLocation = m_gridManager->GetTileLocation(_coords);
	
	TObjectPtr<AGridObject> spawnedGridActor = Cast<AGridObject>(GetWorld()->SpawnActor(_gridObjectBP, &spawnPointLocation));
	if (!ensureMsgf(spawnedGridActor != nullptr, TEXT("ASimulationManager::AddSimulationObject - Spawned actor is nullptr")))
	{
		return false;
	}
	gridTeamObjectArray.Add(spawnedGridActor);

	GridID newID = m_uniqueGridObjectIDs++;
	spawnedGridActor->SetGridID(newID);
	spawnedGridActor->AttachToActor(m_gridManager, FAttachmentTransformRules::KeepWorldTransform);
	spawnedGridActor->SetActorLabel(FString::Format(TEXT("GridObject_{0}_{1}"), {UEnum::GetDisplayValueAsText(_team).ToString(),newID}));
	if (UStaticMeshComponent* mesh = spawnedGridActor->GetComponentByClass<UStaticMeshComponent>())
	{
		mesh->SetCustomPrimitiveDataFloat(0, static_cast<int>(_team));
	}

	//TODO: Move magic numbers into dataAsset
	FGridObjectSimulationData simulationData(_coords, 1, _team, 5, 2, 0, newID);
	m_simulationStateData.Add(simulationData);
	m_IDtoObjectMap.Add(simulationData.m_gridID, spawnedGridActor);
	
	return true;
}

void ASimulationManager::RemoveSimulationObject(GridID _id)
{
	if (TObjectPtr<AGridObject> gridObject = m_IDtoObjectMap.FindRef(_id))
	{
		//TODO: Call a method on actor to do death VFX and stuff
		gridObject->Destroy();
		m_IDtoObjectMap.Remove(_id);
	}
}

FIntVector2 ASimulationManager::GetClosestEnemyGridDataCoords(const FGridObjectSimulationData& _simulationData, const TArray<FGridObjectSimulationData>& _simulationState) const
{
	if (m_gridManager == nullptr || m_gridManager->m_gridSystem == nullptr)
	{
		return FIntVector2::NoneValue;
	}
	
	int closestTileDistance = std::numeric_limits<int>::max();
	FIntVector2 closestCoords = FIntVector2::NoneValue;
	for (auto otherSimData : _simulationState)
	{
		if (_simulationData.m_team == otherSimData.m_team)
		{
			continue;
		}

		const int tileDistance = m_gridManager->m_gridSystem->GetDistance(_simulationData.m_coords, otherSimData.m_coords);
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
		if (m_gridManager->m_gridSystem->GetDistance(coord, _to) < crtDist
			&& !m_simulationGridState.Contains(coord))
		{
			return {coord};
		}
	}

	for (auto coord : m_gridManager->m_gridSystem->GetNeighbourCoords(_from))
	{
		if (m_gridManager->m_gridSystem->GetDistance(coord, _to) <= crtDist
			&& !m_simulationGridState.Contains(coord))
		{
			return {coord};
		}
	}
	//----------------------
	
	//A* here
	return {};
}
UE_ENABLE_OPTIMIZATION

