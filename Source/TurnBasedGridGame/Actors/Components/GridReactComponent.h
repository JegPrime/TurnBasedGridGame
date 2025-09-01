// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GridReactComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TURNBASEDGRIDGAME_API UGridReactComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UGridReactComponent();
	void Setup(TObjectPtr<UStaticMeshComponent> _meshComponent);

	void React();

private:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	void UpdateReact(float _deltaTime);

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> m_meshComponent;

	float m_elapsedTimeSinceLastMove = 0.0f;
	float m_reactTime = 0.2f;
};
