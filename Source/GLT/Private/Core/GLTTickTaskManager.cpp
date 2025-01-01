// Fill out your copyright notice in the Description page of Project Settings.

#include "Core/GLTTickTaskManager.h"
#include "Core/GLTickableGameObject.h"

IGLTTickTaskManager& IGLTTickTaskManager::Get()
{
    return FGLTTickTaskManager::Get();
}

void FGLTTickTaskManager::Tick(float DeltaSeconds)
{
    // Pending Objects
    {
        FScopeLock Lock(&TickableObjectsPendingLock);
        if (!TickableObjectsPendingAdd.IsEmpty())
        {
            TickableObjects.Append(TickableObjectsPendingAdd);
        }
        if (!TickableObjectsPendingRemove.IsEmpty())
        {
            auto ObjectsToRemove = TickableObjects.Intersect(TickableObjectsPendingRemove);
            for (auto ObjectToRemove : ObjectsToRemove)
            {
                TickableObjects.Remove(ObjectToRemove);
            }
        }

        TickableObjectsPendingAdd.Reset();
        TickableObjectsPendingRemove.Reset();
    }

    if (!GWorld || !GWorld->IsGameWorld()) return;

    const float ResultDeltaTime = DeltaSeconds * GWorld->GetWorldSettings()->GetEffectiveTimeDilation();
    for (FGLTickableGameObject* TickableObject : TickableObjects)
    {
        TickableObject->GLTTick(ResultDeltaTime);
    }
}

void FGLTTickTaskManager::AddTickableObject(FGLTickableGameObject* Object)
{
    FScopeLock Lock(&TickableObjectsPendingLock);
    TickableObjectsPendingAdd.Add(Object);
}

void FGLTTickTaskManager::RemoveTickableObject(FGLTickableGameObject* Object)
{
    FScopeLock Lock(&TickableObjectsPendingLock);
    TickableObjectsPendingRemove.Add(Object);
}