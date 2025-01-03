// Fill out your copyright notice in the Description page of Project Settings.

#include "Core/GLTRunnable.h"
#include "RenderTimer.h"
#include "Core/GLTCore.h"
#include "Core/GLTTickTaskManager.h"
#include "HAL/ThreadHeartBeat.h"

#if DO_CHECK
std::atomic<int8> FGLTRunnable::Debug_ThreadCounter = 0;
#endif

FGLTRunnable::FGLTRunnable()
{
    GLTDeltaSeconds = FApp::GetDeltaTime();
    EndFrameEvent = nullptr;
    
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
    EndFrameEvent = FPlatformProcess::GetSynchEventFromPool();
    return true;
}

uint32 FGLTRunnable::Run()
{
    while (StopThreadCounter == 0 && !IsEngineExitRequested())
    {
        FThreadHeartBeat::Get().HeartBeat();

        uint32 CurrentTime = FPlatformTime::Cycles();

        constexpr TCHAR RegionName[] = TEXT("GLCycle");
        TRACE_BEGIN_REGION(RegionName);

        {
            TRACE_CPUPROFILER_EVENT_SCOPE(GLT::PreTickTasks);
            FTaskGraphInterface::Get().ProcessThreadUntilIdle(ENamedThreads::GLThread);
        }

        FMultiFutureHandle Handle;
        if (FGLTTickTaskManager::CanTick())
        {
            TRACE_CPUPROFILER_EVENT_SCOPE(GLT::Tick);
            Handle = FGLTTickTaskManager::Get().Tick(GLTDeltaSeconds);
        }

        bool bNeedPostTickTaskProcess = !Handle.IsReady();
        
        {
            TRACE_CPUPROFILER_EVENT_SCOPE(GLT::Tasks);
            FTaskGraphInterface::Get().ProcessThreadUntilIdle(ENamedThreads::GLThread);
        }

        if (bNeedPostTickTaskProcess)
        {
            TRACE_CPUPROFILER_EVENT_SCOPE(GLT::PostAllTickTasks);
            Handle.Wait();
            FTaskGraphInterface::Get().ProcessThreadUntilIdle(ENamedThreads::GLThread);
        }
        
        uint32 GLTWorkTime = FPlatformTime::Cycles() - CurrentTime;
        if (GLTWorkTime == 0 || GGameThreadTime == 0)
        {
            // Minimum delay of the thread until it is tied to the GT execution time
            FPlatformProcess::Sleep(0.005f);
        }
        else
        {
            GLTToGTCycleRatio = FPlatformTime::ToMilliseconds(GLTWorkTime) / FPlatformTime::ToMilliseconds(GGameThreadTime);

            // We should not allow GLT to be executed more often than GameThread.
            if (GLTToGTCycleRatio < 1.f)
            {
                // Safe delegate subscribe
                AsyncTask(ENamedThreads::GameThread, [this]()
                {
                    FCoreDelegates::OnEndFrame.AddRaw(this, &FGLTRunnable::OnEndFrame); 
                });
                
                EndFrameEvent->Wait();
            }
        }

        GLTDeltaSeconds = FPlatformTime::ToSeconds(FPlatformTime::Cycles() - CurrentTime);
        
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

void FGLTRunnable::OnEndFrame()
{
    FCoreDelegates::OnEndFrame.RemoveAll(this);
    EndFrameEvent->Trigger();
}