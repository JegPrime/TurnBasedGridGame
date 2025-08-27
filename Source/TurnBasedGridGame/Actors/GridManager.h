// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridManager.generated.h"

UCLASS()
class TURNBASEDGRIDGAME_API AGridManager : public AActor
{
	GENERATED_BODY()

public:
	AGridManager();

private:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere)
	UStaticMesh* m_tileMesh;
};