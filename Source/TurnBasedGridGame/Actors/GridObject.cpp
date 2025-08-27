// Fill out your copyright notice in the Description page of Project Settings.


#include "GridObject.h"


// Sets default values
AGridObject::AGridObject()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AGridObject::BeginPlay()
{
	Super::BeginPlay();
}

void AGridObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

