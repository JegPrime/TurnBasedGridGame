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

// Called every frame
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
	if (ensureMsgf(m_gridManager != nullptr, TEXT("ASimulationManager::Initialize - m_gridManager failed initialization")))
	{
		m_gridManager->OnGridGenerated.AddDynamic(this, &ASimulationManager::OnGridGenerated);
		m_gridManager->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
	}
}

void ASimulationManager::Initialize()
{
	PrimaryActorTick.bCanEverTick = true;
	
	//TODO: Add initial simulation objects here
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

