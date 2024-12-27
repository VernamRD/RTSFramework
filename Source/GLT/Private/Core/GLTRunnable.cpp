// Fill out your copyright notice in the Description page of Project Settings.

#include "Core/GLTRunnable.h"

#if DO_CHECK
std::atomic<int8> FGLTRunnable::Debug_ThreadCounter = 0;
#endif

FGLTRunnable::FGLTRunnable()
{
    checkf(Debug_ThreadCounter == 0, TEXT("Attempt to create a second FGLTRunnable"));

    ++Debug_ThreadCounter;
}

FGLTRunnable::~FGLTRunnable()
{
    --Debug_ThreadCounter;
}

bool FGLTRunnable::Init()
{
    FTaskGraphInterface::Get().AttachToThread(ENamedThreads::GLThread);
    return true;
}

uint32 FGLTRunnable::Run()
{
    while (StopThreadCounter == 0 && !IsEngineExitRequested())
    {
        TRACE_CPUPROFILER_EVENT_SCOPE(GLT::Tick);
        constexpr TCHAR RegionName[] = TEXT("GameLogicCycle");
        TRACE_BEGIN_REGION(RegionName);
        
        FTaskGraphInterface::Get().ProcessThreadUntilIdle(ENamedThreads::GLThread);
        FPlatformProcess::Sleep(0.005f);

        TRACE_END_REGION(RegionName);
    }

    FTaskGraphInterface::Get().ProcessThreadUntilIdle(ENamedThreads::GLThread);

    return 0;
}

void FGLTRunnable::Stop()
{
    ++StopThreadCounter;
}

void FGLTRunnable::Exit()
{
    FRunnable::Exit();
}