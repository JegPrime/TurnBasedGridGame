// Fill out your copyright notice in the Description page of Project Settings.


#include "GridMoveComponent.h"

#include "Kismet/KismetMathLibrary.h"


UGridMoveComponent::UGridMoveComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UGridMoveComponent::StartMove(const FVector& _oldLocation, const FVector& _newLocation, const float _time)
{
	if (!ensureMsgf(_time > 0, TEXT("UGridMoveComponent::StartMove - _time is invalid")))
	{
		return;
	}
	
	PrimaryComponentTick.SetTickFunctionEnable(true);
	m_oldLocation = _oldLocation;
	m_newLocation = _newLocation;
	m_time = _time;

	GetOwner()->SetActorLocation(_oldLocation);
	GetOwner()->SetActorRotation(UKismetMathLibrary::FindLookAtRotation(_oldLocation, _newLocation));
}

void UGridMoveComponent::UpdateMove(const float _deltaTime)
{
	m_elapsedTimeSinceLastMove += _deltaTime;
	if (m_elapsedTimeSinceLastMove >= m_time)
	{
		m_elapsedTimeSinceLastMove = m_time;
		PrimaryComponentTick.SetTickFunctionEnable(false);
		//TODO: Maybe broadcast an event that we're done moving if needed
	}
	const FVector currentLocation = FMath::Lerp(m_oldLocation, m_newLocation, m_elapsedTimeSinceLastMove / m_time);
	GetOwner()->SetActorLocation(currentLocation);
}

void UGridMoveComponent::BeginPlay()
{
	Super::BeginPlay();

	//Disabling tick here as constructor doesn't want to work properly for this use case
	PrimaryComponentTick.SetTickFunctionEnable(false);
}

void UGridMoveComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	UpdateMove(DeltaTime);	
}

