// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IGLTGameObject.h"

enum class EGLTTickType
{
    Default,
    Async,

    // ONLY FOR COUNT ENUM FIELDS
    COUNT,
};

class GLT_API IGLTTickTaskManager
{
public:
    virtual ~IGLTTickTaskManager() {}
    static IGLTTickTaskManager& Get();
    
    virtual void RegisterGLTTickableGameObject(IGLTGameObject* Object) {}
    virtual void UnregisterGLTTickableGameObject(IGLTGameObject* Object) {}
    
    virtual void AddTickableObject(EGLTTickType TickType, IGLTGameObject* Object) {}
    virtual void RemoveTickableObject(EGLTTickType TickType, IGLTGameObject* Object) {}
};
