// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IGLTGameObject.h"
#include "IGLTTickTaskManager.h"

GLT_API class FGLTGameObject : public IGLTGameObject
{
public:
    FGLTGameObject(){}
    
    virtual ~FGLTGameObject() override
    {
        UnregisterGLTObject();
    }

    template <typename UObjectParent = UObject UE_REQUIRES(TIsDerivedFrom<UObjectParent, UObject>::IsDerived)>
    void RegisterGLTObject(UObjectParent* ThisObject)
    {
        static_assert(TIsDerivedFrom<UObjectParent, FGLTGameObject>::IsDerived, "Object is not derived from FGLTTickableGameObject");

        if (ThisObject->HasAnyFlags(RF_ArchetypeObject | RF_ClassDefaultObject)) return;

#if WITH_EDITOR
        if (ThisObject->GetWorld()->IsPreviewWorld()) return;
#endif
        
        IGLTTickTaskManager::Get().RegisterGLTTickableGameObject(this);
    }
    template <typename ParentType UE_REQUIRES(!TIsDerivedFrom<ParentType, UObject>::IsDerived)>
    void RegisterGLTObject(ParentType* ThisObject)
    {
        IGLTTickTaskManager::Get().RegisterGLTTickableGameObject(this);
    }
    
    void UnregisterGLTObject()
    {
        IGLTTickTaskManager::Get().UnregisterGLTTickableGameObject(this);
    }
};