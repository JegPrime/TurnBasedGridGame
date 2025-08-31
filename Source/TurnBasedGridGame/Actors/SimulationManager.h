// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SimulationManager.generated.h"

class AGridManager;

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

	UFUNCTION()
	void OnGridGenerated();
	
	//Used in case we don't have a grid already actor already in the world.
	UPROPERTY(EditAnywhere, DisplayName="Grid BP Reference", Category="SimulationManager")
	TSubclassOf<AGridManager> m_gridManagerBP;
	UPROPERTY(EditAnywhere, DisplayName="Grid Actor", Category="SimulationManager")
	TObjectPtr<AGridManager> m_gridManager;
	UPROPERTY(EditDefaultsOnly)
	float m_simulationTickTime = 0.1f;
	
private:
	void CreateGridManager();
	void Initialize();
	void AddSimulationObject();
	void MoveSimulationObject();
	void RemoveSimulationObject();
};
