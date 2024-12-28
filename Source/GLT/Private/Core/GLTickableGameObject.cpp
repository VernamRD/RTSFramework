// Fill out your copyright notice in the Description page of Project Settings.

#include "Core/GLTickableGameObject.h"
#include "Core/IGLTTickTaskManager.h"

FGLTickableGameObject::FGLTickableGameObject()
{
    IGLTTickTaskManager::Get().AddTickableObject(this);
}

FGLTickableGameObject::~FGLTickableGameObject()
{
    IGLTTickTaskManager::Get().RemoveTickableObject(this);
}