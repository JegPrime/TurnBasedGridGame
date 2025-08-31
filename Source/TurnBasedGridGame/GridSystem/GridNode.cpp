// Fill out your copyright notice in the Description page of Project Settings.


#include "GridNode.h"

FGridNode::FGridNode()
{
	m_gridX = 0;
	m_gridY = 0;
}

FGridNode::FGridNode(int _gridX, int _gridY)
{
	m_gridX = _gridX;
	m_gridY = _gridY;
}