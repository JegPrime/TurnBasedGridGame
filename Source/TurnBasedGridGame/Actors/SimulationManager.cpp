// Fill out your copyright notice in the Description page of Project Settings.


#include "SimulationManager.h"
#include "GridManager.h"


ASimulationManager::ASimulationManager()
{
	PrimaryActorTick.bCanEverTick = false;
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

	//Spawn Teams
	for (int team = 0; team < static_cast<int>(EGridObjectTeam::Count); ++team)
	{
		const EGridObjectTeam gridTeam = static_cast<EGridObjectTeam>(team);
		TArray<TObjectPtr<AGridObject>>& gridTeamObjectArray = m_gridObjectsMap.FindOrAdd(gridTeam);
		if (FIntVector2* startPos = m_startingAreaCenters.Find(gridTeam))
		{
			TArray<FIntVector2> potentialSpawnCoords = m_gridManager->m_gridSystem->GetValidCoordsWithinRange(*startPos, m_startingAreaRange);
			for (int i = 0; i < m_startingGridObjectsPerTeam; ++i)
			{
				if (potentialSpawnCoords.IsEmpty())
				{
					break;
				}
				const int randomIndex = FMath::RandRange(0, potentialSpawnCoords.Num() - 1);
				const FIntVector2 spawnCoords = potentialSpawnCoords[randomIndex];
				const FVector spawnPointLocation = m_gridManager->GetTileLocation(spawnCoords);
			
				const TSubclassOf<AGridObject> randomGridActorBP = m_gridObjectBP[FMath::RandRange(0, m_gridObjectBP.Num() - 1)];
				TObjectPtr<AGridObject> spawnedGridActor = Cast<AGridObject>(GetWorld()->SpawnActor(randomGridActorBP, &spawnPointLocation));
				if (ensureMsgf(spawnedGridActor != nullptr, TEXT("ASimulationManager::Initialize - Spawned actor is nullptr")))
				{
					potentialSpawnCoords.RemoveAtSwap(randomIndex);
					gridTeamObjectArray.Add(spawnedGridActor);

					if (UStaticMeshComponent* mesh = spawnedGridActor->GetComponentByClass<UStaticMeshComponent>())
					{
						mesh->SetCustomPrimitiveDataFloat(0, team);
					}
				}
			}
		}
	}
}

void ASimulationManager::AddSimulationObject()
{
	
}

void ASimulationManager::MoveSimulationObject()
{
	
}

void ASimulationManager::RemoveSimulationObject()
{
	
}

