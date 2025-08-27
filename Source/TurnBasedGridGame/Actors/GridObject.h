// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridObject.generated.h"

UCLASS()
class TURNBASEDGRIDGAME_API AGridObject : public AActor
{
	GENERATED_BODY()

public:
	AGridObject();

	virtual bool IsBlocker() { return true; }

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
};
