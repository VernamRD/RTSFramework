// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GLTRunnable.h"

constexpr int32 GLTThreadID = 3;

namespace GLT
{
    enum class EGLTAsyncExecution
    {
        GLTMainThread,
        GLTWorkerThreads
    };

    class IGLT
    {

    public:
        static void Initialize()
        {
            UE_CALL_ONCE([]()
                {
                SCOPED_BOOT_TIMING("StartRenderingThread");
                GameLogicRunnable = new FGLTRunnable();
                GameLogicMainThread = FRunnableThread::Create(GameLogicRunnable, TEXT("GameLogicThread"), 0, TPri_AboveNormal);
                });
        }

        static void Deinitialize()
        {
            UE_CALL_ONCE([]()
                {
                check(GameLogicMainThread);
                GameLogicMainThread->Kill();
                delete GameLogicMainThread;
                delete GameLogicRunnable;
                });
        }

    private:
        inline static FRunnableThread* GameLogicMainThread = nullptr;
        inline static FRunnable* GameLogicRunnable = nullptr;
    };
}