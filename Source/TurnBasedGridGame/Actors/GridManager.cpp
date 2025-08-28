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
					spawnedActor->SetActorScale3D(FVector(m_gridTileScale));
					spawnedActor->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
				}
			}
		}
	}
}

void AGridManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	delete m_gridSystem;
	m_gridSystem = nullptr;
	
	Super::EndPlay(EndPlayReason);
}

void AGridManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

const FVector AGridManager::GetTileLocation(const FGridNode& _gridNode) const
{
	FVector tileLocation = m_gridSystem->GetRelativeLocationForNode(_gridNode);
	tileLocation *= m_gridTileSize * m_gridTileScale;
	return tileLocation + this->GetActorLocation();
}

