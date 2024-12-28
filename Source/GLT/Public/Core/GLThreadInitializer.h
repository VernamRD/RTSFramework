// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GLThreadProvider.h"
#include "GLTRunnable.h"

class GLT_API FGLThreadInitializer : public IGLTThreadProvider
{
public:
    static void Initialize()
    {
        UE_CALL_ONCE([]()
            {
            SCOPED_BOOT_TIMING("StartGameLogicThread");
            GameLogicRunnable = new FGLTRunnable();
            GameLogicRunnableThread = FRunnableThread::Create(GameLogicRunnable, TEXT("GameLogicThread"), 0, TPri_AboveNormal);
            });
    }

    static void Deinitialize()
    {
        UE_CALL_ONCE([]()
            {
            check(GameLogicRunnableThread);
            GameLogicRunnableThread->Kill();
            
            delete GameLogicRunnableThread;
            GameLogicRunnableThread = nullptr;
            
            delete GameLogicRunnable;
            GameLogicRunnable = nullptr;
            });
    }
};
