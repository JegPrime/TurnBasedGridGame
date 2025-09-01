// Fill out your copyright notice in the Description page of Project Settings.


#include "GridObject.h"
#include "Components/GridMoveComponent.h"


// Sets default values
AGridObject::AGridObject()
{
	PrimaryActorTick.bCanEverTick = false;
	TObjectPtr<USceneComponent> sceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("IAmRoot"));
	SetRootComponent(sceneComponent);
	m_moveComponent = CreateDefaultSubobject<UGridMoveComponent>("Move Component");
	m_staticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("Static Mesh Component");
	m_staticMeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	m_reactComponent = CreateDefaultSubobject<UGridReactComponent>("React Component");
	m_reactComponent->Setup(m_staticMeshComponent);
}

void AGridObject::Move(const FVector& _newLocation, const float _time)
{
	if (ensureMsgf(m_moveComponent != nullptr, TEXT("AGridObject::Move - m_moveComponent isn't initialized")))
	{
		m_moveComponent->StartMove(GetActorLocation(), _newLocation, _time);
	}
}

void AGridObject::GetHit()
{
	if (ensureMsgf(m_reactComponent != nullptr, TEXT("AGridObject::GetHit - m_reactComponent isn't initialized")))
	{
		m_reactComponent->React();
	}
}

