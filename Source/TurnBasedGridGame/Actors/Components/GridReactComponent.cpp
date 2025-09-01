// Fill out your copyright notice in the Description page of Project Settings.


#include "GridReactComponent.h"


UGridReactComponent::UGridReactComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UGridReactComponent::Setup(TObjectPtr<UStaticMeshComponent> _meshComponent)
{
	m_meshComponent = _meshComponent;
}

void UGridReactComponent::React()
{
	m_elapsedTimeSinceLastMove = 0.0f;
	PrimaryComponentTick.SetTickFunctionEnable(true);
}


void UGridReactComponent::BeginPlay()
{
	Super::BeginPlay();

	PrimaryComponentTick.SetTickFunctionEnable(false);
}


// Called every frame
void UGridReactComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateReact(DeltaTime);
}

void UGridReactComponent::UpdateReact(float _deltaTime)
{
	m_elapsedTimeSinceLastMove += _deltaTime;
	if (m_elapsedTimeSinceLastMove >= m_reactTime)
	{
		m_elapsedTimeSinceLastMove = m_reactTime;
		PrimaryComponentTick.SetTickFunctionEnable(false);
	}
	m_meshComponent->SetCustomPrimitiveDataFloat(1, m_elapsedTimeSinceLastMove/m_reactTime);
}

