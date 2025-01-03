// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class GLT_API IGLTGameObject
{
public:
    virtual ~IGLTGameObject() {}
    
    virtual void GLTTick(float DeltaSeconds) {}
    virtual bool IsGLTTickable() const { return false; }
    virtual void GLTAsyncTick(float DeltaSeconds) {}
    virtual bool IsGLTAsyncTickable() const { return false; }
};