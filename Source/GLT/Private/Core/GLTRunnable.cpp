// Fill out your copyright notice in the Description page of Project Settings.

#include "Core/GLTRunnable.h"
#include "RenderTimer.h"
#include "Core/GLTCore.h"
#include "Core/GLTTickTaskManager.h"

#if DO_CHECK
std::atomic<int8> FGLTRunnable::Debug_ThreadCounter = 0;
#endif

FGLTRunnable::FGLTRunnable()
{
#if DO_CHECK
    checkf(Debug_ThreadCounter == 0, TEXT("Attempt to create a second FGLTRunnable"));
    
    ++Debug_ThreadCounter;
#endif
}

FGLTRunnable::~FGLTRunnable()
{
#if DO_CHECK
    --Debug_ThreadCounter;
#endif
}

bool FGLTRunnable::Init()
{
    FTaskGraphInterface::Get().AttachToThread(ENamedThreads::GLThread);
    bPendingNewTimeModifier = true;
    NewTimeModifier = 1.0f;
    return true;
}

uint32 FGLTRunnable::Run()
{
    while (StopThreadCounter == 0 && !IsEngineExitRequested())
    {
        ApplyTimeModifier();

        uint32 CurrentTime = FPlatformTime::Cycles();

        constexpr TCHAR RegionName[] = TEXT("GLCycle");
        TRACE_BEGIN_REGION(RegionName);

        {
            TRACE_CPUPROFILER_EVENT_SCOPE(GLT::Tick);
            FGLTTickTaskManager::Get().Tick(TimeModifier);
        }
        
        FTaskGraphInterface::Get().ProcessThreadUntilIdle(ENamedThreads::GLThread);
        FPlatformProcess::Sleep(0.005f);

        TRACE_END_REGION(RegionName);

        uint32 GLTTime = FPlatformTime::Cycles() - CurrentTime;
        GTToGLTCycleRatio = GGameThreadTime / GLTTime;
    }

    FTaskGraphInterface::Get().ProcessThreadUntilIdle(ENamedThreads::GLThread);

    return 0;
}

void FGLTRunnable::Stop()
{
    ++StopThreadCounter;
}

void FGLTRunnable::SetTimeModifier(float InNewTimeModifier)
{
    NewTimeModifier = InNewTimeModifier;
    bPendingNewTimeModifier = true;
}

void FGLTRunnable::Exit()
{
    FRunnable::Exit();
}

void FGLTRunnable::ApplyTimeModifier()
{
    if (bPendingNewTimeModifier.load() == false) return;

    TimeModifier = NewTimeModifier.load();
    bPendingNewTimeModifier = false;
}