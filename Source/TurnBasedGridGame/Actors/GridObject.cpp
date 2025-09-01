// Fill out your copyright notice in the Description page of Project Settings.


#include "GridObject.h"
#include "Components/GridMoveComponent.h"


// Sets default values
AGridObject::AGridObject()
{
	PrimaryActorTick.bCanEverTick = false;
	 m_moveComponent = CreateDefaultSubobject<UGridMoveComponent>("Move Component");
}

void AGridObject::Move(const FVector& _newLocation, const float _time)
{
	if (ensureMsgf(m_moveComponent != nullptr, TEXT("AGridObject::Move - m_moveComponent isn't initialized")))
	{
		m_moveComponent->StartMove(GetActorLocation(), _newLocation, _time);
	}
}

