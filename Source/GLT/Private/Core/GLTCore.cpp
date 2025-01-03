// Fill out your copyright notice in the Description page of Project Settings.

#include "GLT/Public/Core/GLTCore.h"

#include "Core/GLThreadProvider.h"

bool IsGLTValid()
{
    return IGLTThreadProvider::GetGameLogicRunnableThread() != nullptr && IGLTThreadProvider::GetGameLogicRunnable() != nullptr;
}

float GetGLTToGTCycleRatio()
{
    if (!IsGLTValid()) return TNumericLimits<float>::Min();

    return IGLTThreadProvider::GetGameLogicRunnable()->GetGLTToGTRatio();
}

bool IsInGLThread()
{
    if (!IsGLTValid()) return false;
    
    return FPlatformTLS::GetCurrentThreadId() == IGLTThreadProvider::GetGameLogicRunnableThread()->GetThreadID();
}

float GetGLTDeltaSeconds()
{
    if (!IsGLTValid()) return false;
    
    return IGLTThreadProvider::GetGameLogicRunnable()->GetGLTDeltaSeconds();
}

TArray<TInterval<int32>> CalculatePoolPerWorker(int32 NumWorks, int32 WorkersCount)
{
    TArray<TInterval<int32>> PoolPerWorkers;

    if (WorkersCount > NumWorks)
    {
        WorkersCount = NumWorks;
        PoolPerWorkers.AddDefaulted(WorkersCount);

        int32 WorkIndex = 0;
        for (TInterval<int32>& PoolForWorker : PoolPerWorkers)
        {
            PoolForWorker = TInterval(WorkIndex, WorkIndex + 1);
            ++WorkIndex;
        }

        return MoveTemp(PoolPerWorkers);
    }

    PoolPerWorkers.AddDefaulted(WorkersCount);

    TArray<int32> PoolSizePerWorker;
    PoolSizePerWorker.AddDefaulted(WorkersCount);

    // The amount of work is fair for any worker
    int32 WorkPerWorker = NumWorks / WorkersCount;
    // The amount of work that needs to be allocated manually when assigning a pool
    int32 RemainingWorks = NumWorks % WorkersCount;

    // Evenly distribute the amount of work for each worker
    for (int32 WorkerIndex = 0, JobIndex = 0; WorkerIndex < WorkersCount; ++WorkerIndex)
    {
        int32 LastJobIndex = JobIndex + WorkPerWorker;

        // Adding the rest of the work
        if (RemainingWorks > 0)
        {
            ++LastJobIndex;
            --RemainingWorks;
        }

        // Job index range 
        PoolPerWorkers[WorkerIndex] = TInterval(JobIndex, JobIndex + WorkPerWorker);
        JobIndex = LastJobIndex;
    }

    return MoveTemp(PoolPerWorkers);
}

FMultiFutureHandle ParallelDoWork(int32 NumWorks, TFunction<void(int32 WorkerIndex, int32 WorkIndex)>&& Callable)
{
    TRACE_CPUPROFILER_EVENT_SCOPE(GLT::ParallelDoWork);

    FMultiFutureHandle MultiFutureHandle;
    
    TArray<TInterval<int32>> PoolsPerWorker = CalculatePoolPerWorker(NumWorks);
    const int32 WorkersCount = PoolsPerWorker.Num();

    for (int32 WorkerIndex = 0; WorkerIndex < WorkersCount; ++WorkerIndex)
    {
        TInterval<int32> WorkerPool = PoolsPerWorker[WorkerIndex];
        auto Future = Async(EAsyncExecution::TaskGraph, [WorkerIndex, WorkerPool, Callable]()
        {
            TRACE_CPUPROFILER_EVENT_SCOPE(GLT::ForEachWorker);

            const int32 PoolDownBorder = WorkerPool.Min;
            const int32 PoolUpBorder = WorkerPool.Max;

            for (int32 Index = PoolDownBorder; Index < PoolUpBorder; ++Index)
            {
                Callable(WorkerIndex, Index);
            }
        });

        MultiFutureHandle.InsertFuture(MoveTemp(Future));
    }

    return MoveTemp(MultiFutureHandle);
}

FMultiFutureHandle ParallelPooledDoWork(int32 NumWorks, TFunction<void(int32 WorkerIndex, int32 WorkStartIndex, int32 WorkLastIndex)>&& Callable)
{
    TRACE_CPUPROFILER_EVENT_SCOPE(GLT::ParallelDoWork);

    FMultiFutureHandle MultiFutureHandle;
    
    TArray<TInterval<int32>> PoolsPerWorker = CalculatePoolPerWorker(NumWorks);
    const int32 WorkersCount = PoolsPerWorker.Num();

    for (int32 WorkerIndex = 0; WorkerIndex < WorkersCount; ++WorkerIndex)
    {
        TInterval<int32> WorkerPool = PoolsPerWorker[WorkerIndex];
        auto Future = Async(EAsyncExecution::TaskGraph, [WorkerIndex, WorkerPool, Callable]()
        {
            TRACE_CPUPROFILER_EVENT_SCOPE(GLT::ForEachWorker);

            const int32 PoolDownBorder = WorkerPool.Min;
            const int32 PoolUpBorder = WorkerPool.Max;

            Callable(WorkerIndex, PoolDownBorder, PoolUpBorder);
        });

        MultiFutureHandle.InsertFuture(MoveTemp(Future));
    }

    return MoveTemp(MultiFutureHandle);
}
