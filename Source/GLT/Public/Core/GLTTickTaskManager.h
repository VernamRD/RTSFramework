// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GLTCore.h"
#include "IGLTTickTaskManager.h"

class FGLTickableGroup : public TSharedFromThis<FGLTickableGroup>
{
public:
    FGLTickableGroup() = default;

    void Add(IGLTGameObject* Object);
    void Remove(IGLTGameObject* Object);
    void Resolve();

    int32 Num() const { return Objects.Num(); }
    IGLTGameObject& Get(int32 Index) const { return *CachedObjectsArray[Index]; }

    FORCEINLINE auto begin() { return CachedObjectsArray.begin(); }
    FORCEINLINE auto begin() const { return CachedObjectsArray.begin(); }
    FORCEINLINE auto end() { return CachedObjectsArray.end(); }
    FORCEINLINE auto end() const { return CachedObjectsArray.end(); }

private:
    TArray<IGLTGameObject*> CachedObjectsArray;
    TSet<IGLTGameObject*> Objects;

    FCriticalSection ObjPendingLock;
    TSet<IGLTGameObject*> ObjPendingAdd;
    TSet<IGLTGameObject*> ObjPendingRemove;
};

class FGLTTickTaskManager : public IGLTTickTaskManager
{
public:
    static FGLTTickTaskManager& Get()
    {
        static FGLTTickTaskManager Singleton;
        return Singleton;
    }

    FGLTTickTaskManager();

    static bool CanTick()
    {
#if WITH_EDITOR
        if (!GWorld) return false;
        if (GWorld->IsGameWorld()) return true;

        for (const FWorldContext& Context : GEngine->GetWorldContexts())
        {
            if (Context.World() && Context.World()->IsGameWorld()) return true;
        }

        return false;
#else
        return GWorld && GWorld->IsGameWorld();
#endif
    }

    [[nodiscard]] FMultiFutureHandle Tick(float DeltaSeconds);

    virtual void RegisterGLTTickableGameObject(IGLTGameObject* Object) override;
    virtual void UnregisterGLTTickableGameObject(IGLTGameObject* Object) override;

    virtual void AddTickableObject(EGLTTickType TickType, IGLTGameObject* Object) override;
    virtual void RemoveTickableObject(EGLTTickType TickType, IGLTGameObject* Object) override;

private:
    TMap<EGLTTickType, TUniquePtr<FGLTickableGroup>> TickGroups;
    TSet<>
};