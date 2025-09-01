// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GridMoveComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TURNBASEDGRIDGAME_API UGridMoveComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UGridMoveComponent();
	void StartMove(const FVector& _oldLocation, const FVector& _newLocation, const float _time);
	void UpdateMove(const float _deltaTime);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY(VisibleAnywhere)
	FVector m_oldLocation = FVector::ZeroVector;
	UPROPERTY(VisibleAnywhere)
	FVector m_newLocation = FVector::ZeroVector;
	UPROPERTY(VisibleAnywhere)
	float m_time = 0;
	UPROPERTY(VisibleAnywhere)
	float m_elapsedTimeSinceLastMove = 0;
};
