// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/GridReactComponent.h"
#include "GameFramework/Actor.h"
#include "GridObject.generated.h"

class UGridMoveComponent;

UCLASS()
class TURNBASEDGRIDGAME_API AGridObject : public AActor
{
	GENERATED_BODY()

public:
	AGridObject();

	virtual bool IsBlocker() const { return true; }
	void Move(const FVector& _newLocation, const float _time);
	void GetHit();
	int GetGridID() const { return m_gridID; }
	void SetGridID(int _id) { m_gridID = _id; }

protected:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UGridMoveComponent> m_moveComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> m_staticMeshComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UGridReactComponent> m_reactComponent;

	int m_gridID = 0;
};
