// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IGLTTickTaskManager.h"

class FGLTTickTaskManager : public IGLTTickTaskManager
{
public:
    static FGLTTickTaskManager& Get()
    {
        static FGLTTickTaskManager Singleton;
        return Singleton;
    }
    
    void Tick(float TimeModifier);
    virtual void AddTickableObject(FGLTickableGameObject* Object) override;
    virtual void RemoveTickableObject(FGLTickableGameObject* Object) override;
    
private:
    TSet<FGLTickableGameObject*> TickableObjects;
    
    FCriticalSection TickableObjectsPendingLock;
    TSet<FGLTickableGameObject*> TickableObjectsPendingAdd;
    TSet<FGLTickableGameObject*> TickableObjectsPendingRemove;
};

