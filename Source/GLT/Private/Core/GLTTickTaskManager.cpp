// Fill out your copyright notice in the Description page of Project Settings.

#include "Core/GLTTickTaskManager.h"

#include "Core/GLTCore.h"
#include "Core/GLTGameObject.h"

void FGLTickableGroup::Add(IGLTGameObject* Object)
{
    FScopeLock Lock(&ObjPendingLock);
    ObjPendingAdd.Add(Object);
}

void FGLTickableGroup::Remove(IGLTGameObject* Object)
{
    FScopeLock Lock(&ObjPendingLock);
    ObjPendingRemove.Add(Object);
}

void FGLTickableGroup::Resolve()
{
    FScopeLock Lock(&ObjPendingLock);
    if (!ObjPendingAdd.IsEmpty())
    {
        Objects.Append(ObjPendingAdd);
    }
    if (!ObjPendingRemove.IsEmpty())
    {
        auto ObjectsToRemove = Objects.Intersect(ObjPendingRemove);
        for (auto ObjectToRemove : ObjectsToRemove)
        {
            Objects.Remove(ObjectToRemove);
        }
    }

    ObjPendingAdd.Reset();
    ObjPendingRemove.Reset();
    CachedObjectsArray = Objects.Array();
}

IGLTTickTaskManager& IGLTTickTaskManager::Get()
{
    return FGLTTickTaskManager::Get();
}

FGLTTickTaskManager::FGLTTickTaskManager()
{
    int32 EnumCount = static_cast<int32>(EGLTTickType::COUNT);
    TickGroups.Reserve(EnumCount);
    
    for (int32 EnumIndex = 0; EnumIndex < EnumCount; ++EnumIndex)
    {
        EGLTTickType EnumType = static_cast<EGLTTickType>(EnumIndex);
        TickGroups.Add(EnumType, MakeUnique<FGLTickableGroup>());
    }
}

FMultiFutureHandle FGLTTickTaskManager::Tick(float DeltaSeconds)
{
    for (auto& TickGroup : TickGroups)
    {
        TickGroup.Value->Resolve();
    }

    const float ResultDeltaTime = DeltaSeconds * GWorld->GetWorldSettings()->GetEffectiveTimeDilation();

    // Async ticks must be run earlier
    FGLTickableGroup& AsyncTickGroup = *TickGroups[EGLTTickType::Async];
    FMultiFutureHandle AsyncTickHandle = ParallelDoWork(AsyncTickGroup.Num(),
        [&AsyncTickGroup, ResultDeltaTime](int32 WorkerIndex, int32 WorkIndex)
        {
            AsyncTickGroup.Get(WorkIndex).GLTAsyncTick(ResultDeltaTime);
        });

    FGLTickableGroup& DefaultTickGroup = *TickGroups[EGLTTickType::Default];
    for (IGLTGameObject* TickableObject : DefaultTickGroup)
    {
        TickableObject->GLTTick(ResultDeltaTime);
    }

    return MoveTemp(AsyncTickHandle);
}

void FGLTTickTaskManager::RegisterGLTTickableGameObject(IGLTGameObject* Object)
{
    if (Object->IsGLTTickable())
    {
        AddTickableObject(EGLTTickType::Default, Object);
    }
    if (Object->IsGLTAsyncTickable())
    {
        AddTickableObject(EGLTTickType::Async, Object);
    }
}

void FGLTTickTaskManager::UnregisterGLTTickableGameObject(IGLTGameObject* Object)
{
    for (auto& TickGroup : TickGroups)
    {
        TickGroup.Value->Remove(Object);
    }
}

void FGLTTickTaskManager::AddTickableObject(EGLTTickType TickType, IGLTGameObject* Object)
{
    check(TickGroups.Contains(TickType));
    TickGroups[TickType]->Add(Object);
}

void FGLTTickTaskManager::RemoveTickableObject(EGLTTickType TickType, IGLTGameObject* Object)
{
    check(TickGroups.Contains(TickType));
    TickGroups[TickType]->Remove(Object);
}