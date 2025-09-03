// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridObject.h"
#include "GameFramework/Actor.h"
#include "SimulationManager.generated.h"

class AGridManager;

typedef int GridID;

UENUM()
enum class EGridObjectTeam : uint8
{
	Red,
	Blue,
	Count UMETA(Hidden)
};

USTRUCT()
struct FGridObjectSimulationData
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	FIntVector2 m_coords = FIntVector2::ZeroValue;
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	FIntVector2 m_targetCoords = FIntVector2::ZeroValue;
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	int m_moveSpeed = 0;
	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	EGridObjectTeam m_team = EGridObjectTeam::Red;
	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	int m_health = 0;
	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	int m_damage = 0;
	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	int m_range = 0;
	//Uproperty doesn't like typedef
	GridID m_gridID = 0;
};



UCLASS()
class TURNBASEDGRIDGAME_API ASimulationManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASimulationManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(CallInEditor)
	void TestMoveActor();
	UFUNCTION(CallInEditor)
	void TestHitActor();
	UFUNCTION()
	void OnGridGenerated();
	
	//Used in case we don't have a grid already actor already in the world.
	UPROPERTY(EditAnywhere, DisplayName="Grid BP Reference", Category="SimulationManager")
	TSubclassOf<AGridManager> m_gridManagerBP;
	UPROPERTY(EditAnywhere, DisplayName="Grid Actor", Category="SimulationManager")
	TObjectPtr<AGridManager> m_gridManager;
	UPROPERTY(EditAnywhere, DisplayName="Grid ObjectBP", Category="SimulationManager")
	TArray<TSubclassOf<AGridObject>> m_gridObjectBP;
	UPROPERTY(EditAnywhere, DisplayName="Grid Simulation Frequency", Category="SimulationManager") 
	float m_simulationTickTime = 0.1f;
	UPROPERTY(EditAnywhere, DisplayName="Starting Point Locations", Category="SimulationManager|TeamSetup")
	TMap<EGridObjectTeam, FIntVector2> m_startingAreaCenters{{EGridObjectTeam::Red, {10,10}}, {EGridObjectTeam::Blue, {90,90}}};
	UPROPERTY(EditAnywhere, DisplayName="Spawn Range of Start Point", Category="SimulationManager|TeamSetup")
	int m_startingAreaRange = 5;
	UPROPERTY(EditAnywhere, DisplayName="Spawn Object Number", Category="SimulationManager|TeamSetup")
	int m_startingGridObjectsPerTeam = 5;
	
private:
	void CreateGridManager();
	void Initialize();
	void UpdateSimulation();
	void UpdateSimulationMovements(TArray<FGridObjectSimulationData>& simObjects, const TArray<FGridObjectSimulationData>& currentSimulationState);
	void UpdateSimulationAttacks(TArray<FGridObjectSimulationData>& simObjects, const TArray<FGridObjectSimulationData>& currentSimulationState);
	void ReadSimulationData(); //Reads current simulation state and issues commands to update objects accordingly
	bool AddSimulationObject(TSubclassOf<AGridObject> _gridObjectBP, FIntVector2 _coords, EGridObjectTeam _team);
	void MoveSimulationObject();
	void RemoveSimulationObject();
	FIntVector2 GetClosestEnemyGridDataCoords(const FGridObjectSimulationData& _gridObjectData, const TArray<FGridObjectSimulationData>& _currentSimulationState) const;
	TArray<FIntVector2> GetPathToCoords(FIntVector2 _from, FIntVector2 _to) const;

private:
	TMap<EGridObjectTeam, TArray<TObjectPtr<AGridObject>>> m_gridObjectsMap;
	TMap<GridID, TObjectPtr<AGridObject>> m_IDtoObjectMap;
	TMap<FIntVector2, FGridObjectSimulationData> m_simulationData;
	
	GridID m_uniqueGridObjectIDs = 0;
};
