// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class GLT_API FGLTickableGameObject
{
public:
    FGLTickableGameObject();
    virtual ~FGLTickableGameObject();
    
    virtual void GLTTick(float TimeModifier) {}
    virtual void GLTAsyncTick(float TimeModifier) {}
};