// Fill out your copyright notice in the Description page of Project Settings.


#include "GridManager.h"

#if WITH_EDITOR
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#endif

// Sets default values
AGridManager::AGridManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AGridManager::BeginPlay()
{
	Super::BeginPlay();

	ensureMsgf(m_gridSystemType != nullptr, TEXT("AGridManager::BeginPlay - m_gridSystem is nullptr"));
	ensureMsgf(m_tileMeshActorClass != nullptr, TEXT("AGridManager::BeginPlay - m_tileMesh is nullptr"));
	ensureMsgf(m_gridSize > 1, TEXT("AGridManager::BeginPlay - m_gridSize too small"));

	RemoveGrid();
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
	
#if WITH_EDITOR
	DrawMousePositionDebug();
#endif
}

void AGridManager::GenerateGridPreview()
{
	RemoveGrid();
	GenerateGrid();
}

void AGridManager::ClearGridPreview()
{
	RemoveGrid();
}

void AGridManager::GenerateGrid()
{
	if (!ensureMsgf(m_gridSystemType != nullptr, TEXT("AGridManager::GenerateGrid - GridSystemType was not set")))
	{
		return;
	}

	TObjectPtr<UGridSystem> gridSystemPtr = NewObject<UGridSystem>(this, m_gridSystemType, FName("GridSystem"));
	m_gridSystem = gridSystemPtr;
	gridSystemPtr = nullptr;
	if (!ensureMsgf(m_gridSystem != nullptr, TEXT("AGridManager::GenerateGrid - GridSystem was not generated")))
	{
		return;
	}

	//TODO: Add possibility to setup rectangular grid 
	m_gridSystem->Setup(m_gridSize);
	FVector tileScaleCorrection = FVector(m_gridTileScale,
										  m_gridTileScale,
										  1);
	if (const AActor* tileCDO = m_tileMeshActorClass.GetDefaultObject())
	{
		if (UStaticMeshComponent* staticMeshComponent = tileCDO->GetComponentByClass<UStaticMeshComponent>())
		{
			FVector min,max = FVector::ZeroVector;
			staticMeshComponent->GetLocalBounds(min, max);
			tileScaleCorrection *= FVector(1 / FMath::Abs(max.X - min.X),
										   1 / FMath::Abs(max.Y - min.Y),
										   1);
		}
	}
	
	if (m_useSingleMesh)
	{
		const FVector gridExtent = FVector(m_gridSize * m_gridTileScale,
										   m_gridSize * m_gridTileScale,
										   0);
		const FVector modifiedActorLocation = GetActorLocation() + gridExtent / 2;
		AActor* spawnedActor = GetWorld()->SpawnActor(m_tileMeshActorClass, &modifiedActorLocation);
		if (ensureMsgf(spawnedActor != nullptr, TEXT("AGridManager::GenerateGrid - Spawned actor is nullptr")))
		{
			if (UStaticMeshComponent* mesh = spawnedActor->GetComponentByClass<UStaticMeshComponent>())
			{
				const FVector scaledMeshSize = m_gridSize * tileScaleCorrection;
				mesh->SetWorldScale3D(FVector(scaledMeshSize.X, scaledMeshSize.Y, 1));
				mesh->SetCustomPrimitiveDataFloat(0, m_gridSize);
				mesh->SetCustomPrimitiveDataFloat(1, m_gridSize);
			}
			spawnedActor->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
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
				if (ensureMsgf(spawnedActor != nullptr, TEXT("AGridManager::GenerateGrid - Spawned actor is nullptr")))
				{
					spawnedActor->SetActorScale3D(tileScaleCorrection);
					spawnedActor->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
				}
				m_gridSystemActors.Add(spawnedActor);
			}
		}
	}
	
	m_isGridGenerated = true;
	OnGridGenerated.Broadcast();
}

void AGridManager::RemoveGrid()
{
	m_isGridGenerated = false;
	m_gridSystem = nullptr;
	for (auto actor : m_gridSystemActors)
	{
		actor->Destroy();
	}
	CollectGarbage(RF_NoFlags, true);
	m_gridSystemActors.Empty();
}

const FVector AGridManager::GetTileLocation(const FGridNode& _gridNode) const
{
	return GetTileLocation(_gridNode.GetGridPosition());
}

const FVector AGridManager::GetTileLocation(const FIntVector2& _coords) const
{
	if (!ensureMsgf(m_gridSystem != nullptr, TEXT("AGridManager::GetTileLocation - m_gridSystem is nullptr")))
	{
		return FVector::ZeroVector;
	}
	
	const FVector tileLocation = m_gridSystem->GetLocationAtCoords(_coords) * m_gridTileScale;
	return tileLocation + GetActorLocation() + (m_useTileMeshOffset ? m_tileDisplacement : FVector::ZeroVector);
}

const FIntVector2 AGridManager::GetTileCoordsAtLocation(const FVector& _location) const
{
	if (!ensureMsgf(m_gridSystem != nullptr, TEXT("AGridManager::GetTileCoordsAtLocation - m_gridSystem is nullptr")))
	{
		return FIntVector2::ZeroValue;
	}

	const FVector offset = m_useTileMeshOffset || m_useSingleMesh ? FVector::ZeroVector : m_tileDisplacement;
	return m_gridSystem->GetCoordsAtLocation((GetActorLocation() + _location + offset) / m_gridTileScale);
}

#if WITH_EDITOR
void AGridManager::DrawMousePositionDebug() const
{
	//Debug for tile location code
	if (auto playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		FVector WorldLocation;
		FVector WorldDirection;
		if (playerController->DeprojectMousePositionToWorld(WorldLocation, WorldDirection))
		{
			FVector cursorLocation = FMath::LinePlaneIntersection(WorldLocation, WorldLocation + WorldDirection, GetActorLocation(), FVector::UpVector);
			UKismetSystemLibrary::PrintString(this, FString::Format(TEXT("CursorTouchLocation : {0} x {1}"), {cursorLocation.X, cursorLocation.Y}), true, false, FLinearColor::Green, 0.f, "CursorTouchLocation");

			FIntVector2 cursorCoords = GetTileCoordsAtLocation(cursorLocation);
			UKismetSystemLibrary::PrintString(this, FString::Format(TEXT("CursorTileCoords : {0} x {1}"), {cursorCoords.X, cursorCoords.Y}), true, false, FLinearColor::Green, 0.f, "CursorTileCoords");

			FVector cursorTileLocation = GetTileLocation(cursorCoords);
			UKismetSystemLibrary::PrintString(this, FString::Format(TEXT("CursorTileLocation : {0} x {1}"), {cursorTileLocation.X, cursorTileLocation.Y}), true, false, FLinearColor::Green, 0.f, "CursorTileLocation");
		}
	}
}
#endif
