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

	if (ensure(m_tileMesh != nullptr))
    {
    	UE_LOG(LogTemp, Warning, TEXT("AGridManager::AGridManager - m_tileMesh is nullptr"));
    }
}

void AGridManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

