// Fill out your copyright notice in the Description page of Project Settings.

#include "GLT/Public/Core/GLTCore.h"

#include "Core/GLThreadProvider.h"

bool IsGLTValid()
{
    return IGLTThreadProvider::GetGameLogicRunnableThread() != nullptr && IGLTThreadProvider::GetGameLogicRunnable() != nullptr;
}

float GetGTToGLTCycleRatio()
{
    if (!IsGLTValid()) return TNumericLimits<float>::Min();

    return IGLTThreadProvider::GetGameLogicRunnable()->GetGTToGLTRatio();
}

bool IsInGLThread()
{
    if (!IsGLTValid()) return false;
    
    return FPlatformTLS::GetCurrentThreadId() == IGLTThreadProvider::GetGameLogicRunnableThread()->GetThreadID();
}
