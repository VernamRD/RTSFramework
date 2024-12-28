// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GLTRunnable.h"

class IGLTThreadProvider
{
public:
    static const FRunnableThread* GetGameLogicRunnableThread() { return GameLogicRunnableThread; }
    static const FGLTRunnable* GetGameLogicRunnable() { return GameLogicRunnable; }

protected:
    inline static FRunnableThread* GameLogicRunnableThread = nullptr;
    inline static FGLTRunnable* GameLogicRunnable = nullptr;
};