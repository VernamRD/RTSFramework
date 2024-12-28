// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class FGLTickableGameObject;

class GLT_API IGLTTickTaskManager
{
public:
    virtual ~IGLTTickTaskManager() {}
    static IGLTTickTaskManager& Get();

    virtual void AddTickableObject(FGLTickableGameObject* Object) {}
    virtual void RemoveTickableObject(FGLTickableGameObject* Object) {}
};
