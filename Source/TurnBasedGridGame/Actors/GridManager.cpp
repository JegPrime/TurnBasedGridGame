// Fill out your copyright notice in the Description page of Project Settings.


#include "GridManager.h"

// Sets default values
AGridManager::AGridManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AGridManager::BeginPlay()
{
	Super::BeginPlay();

	ensureMsgf(m_tileMeshActorClass != nullptr, TEXT("AGridManager::AGridManager - m_tileMesh is nullptr"));
	ensureMsgf(m_gridSize > 1, TEXT("AGridManager::AGridManager - m_gridSize too small"));

	GenerateGrid();
}

void AGridManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	RemoveGrid();
	
	Super::EndPlay(EndPlayReason);
}

void AGridManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

const FVector AGridManager::GetTileLocation(const FGridNode& _gridNode) const
{
	const FVector tileLocation = m_gridSystem->GetRelativeLocationForNode(_gridNode) * m_gridTileScale;
	return tileLocation + m_tileDisplacement + GetActorLocation();
}

const FIntVector2 AGridManager::GetTileCoordsAtLocation(const FVector& _location) const
{
	return FIntVector2(_location.X / m_gridTileScale, _location.Y / m_gridTileScale);
}

void AGridManager::GenerateGrid()
{
	m_gridSystem = new FGridSystemSquare(m_gridSize);
	if (m_useSingleMesh)
	{
		const FVector gridSize = FVector(	m_gridSize * m_gridTileScale,
											m_gridSize * m_gridTileScale,
											0);
		const FVector actorLocation = GetActorLocation() + gridSize / 2;
		AActor* spawnedActor = GetWorld()->SpawnActor(m_tileMeshActorClass, &actorLocation);
		if (ensureMsgf(spawnedActor != nullptr, TEXT("AGridManager::BeginPlay - Spawned actor is nullptr")))
		{
			if (UStaticMeshComponent* mesh = spawnedActor->GetComponentByClass<UStaticMeshComponent>())
			{
				FVector min,max = FVector::ZeroVector;
				mesh->GetLocalBounds(min, max);
				FVector scaledSizeVector = FVector(	gridSize.X / FMath::Abs(max.X - min.X),
													gridSize.Y / FMath::Abs(max.Y - min.Y),
													1);
				mesh->SetWorldScale3D(scaledSizeVector);
				mesh->SetCustomPrimitiveDataFloat(0, m_gridSize);
				mesh->SetCustomPrimitiveDataFloat(1, m_gridSize);
			}
		}
		m_gridSystemActors.Add(spawnedActor);
	}
	else
	{
		for (auto gridLine : m_gridSystem->m_grid)
		{
			for (auto gridNode : gridLine)
			{
				const FVector tileLocation = GetTileLocation(gridNode);
				AActor* spawnedActor = GetWorld()->SpawnActor(m_tileMeshActorClass, &tileLocation);
				if (ensureMsgf(spawnedActor != nullptr, TEXT("AGridManager::BeginPlay - Spawned actor is nullptr")))
				{
					FVector gridScale = FVector::One();
					if (UStaticMeshComponent* mesh = spawnedActor->GetComponentByClass<UStaticMeshComponent>())
					{
						FVector min,max = FVector::ZeroVector;
						mesh->GetLocalBounds(min, max);
						gridScale = FVector(	1 / FMath::Abs(max.X - min.X),
												1 / FMath::Abs(max.Y - min.Y),
												1);
					}
					spawnedActor->SetActorScale3D(FVector(m_gridTileScale * gridScale.X, m_gridTileScale * gridScale.Y, 1));
					spawnedActor->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
				}
				m_gridSystemActors.Add(spawnedActor);
			}
		}
	}
}

void AGridManager::RemoveGrid()
{
	delete m_gridSystem;
	m_gridSystem = nullptr;

	for (auto actor : m_gridSystemActors)
	{
		actor->Destroy();
	}
	m_gridSystemActors.Empty();
}

