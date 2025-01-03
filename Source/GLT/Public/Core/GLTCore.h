// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

GLT_API bool IsGLTValid();
GLT_API float GetGLTToGTCycleRatio();
GLT_API bool IsInGLThread();
GLT_API float GetGLTDeltaSeconds();

/**
 *  Evenly distributes the specified amount of work to the specified number of workers and creates a range of indexes
 *  
 * @param NumWorks Amount of work that will be evenly distributed among the available number of workers
 * @param WorkersCount Number of workers
 * 
 * @return Pool per worker {StartWorkIndex, LastWorkIndex} in example: {{0, 13}, {13, 26}, {26, 39}} etc.
 * 
 * @remark The pool is specified as the first index and the last + 1 to simplify the counting and use of iterators.
 * @remark For example, if 1 job was assigned to a worker, then his pool will look like: {0, 1}, {5, 6}, {10, 11} etc.
 */
GLT_API TArray<TInterval<int32>> CalculatePoolPerWorker(int32 NumWorks, int32 WorkersCount);

/**
 *  Evenly distributes the specified amount of work to the specified number of workers and creates a range of indexes
 *  
 * @param NumWorks Amount of work that will be evenly distributed among the available number of workers
 * 
 * @return Pool per worker {StartWorkIndex, LastWorkIndex} in example: {{0, 13}, {13, 26}, {26, 39}} etc.
 * @return The number of workers will be equal to GetNumBackgroundThreads()
 * 
 * @remark The pool is specified as the first index and the last + 1 to simplify the counting and use of iterators.
 * @remark For example, if 1 job was assigned to a worker, then his pool will look like: {0, 1}, {5, 6}, {10, 11} etc.
 */
GLT_API FORCEINLINE TArray<TInterval<int32>> CalculatePoolPerWorker(int32 NumWorks)
{
    return CalculatePoolPerWorker(NumWorks, FTaskGraphInterface::Get().GetNumBackgroundThreads());
}

GLT_API class FMultiFutureHandle
{
public:
    /** Default constructor. */
    FMultiFutureHandle() = default;
    /** Deleted copy constructor (futures cannot be copied). */
    FMultiFutureHandle(const FMultiFutureHandle&) = delete;
    /** Deleted copy assignment operator (futures cannot be copied). */
    FMultiFutureHandle& operator=(const FMultiFutureHandle&) = delete;
    /** Move constructor. */
    FMultiFutureHandle(FMultiFutureHandle&&) = default;
    /** Move assignment operator. */
    FMultiFutureHandle& operator=(FMultiFutureHandle&&) = default;
    
    template <typename TFutureType>
    void InsertFuture(TFuture<TFutureType>&& Future)
    {
        Futures.Add(MoveTemp(Future));
    }

    bool IsReady() const
    {
        bool bNotReady = false;
        for (auto& Future : Futures)
        {
            bNotReady |= !Future.IsReady();
        }

        return bNotReady;
    }

    void Wait() const
    {
        for (auto& Future : Futures)
        {
            if (!Future.IsReady())
            {
                Future.Wait();
            }
        }
    }

    bool WaitFor(const FTimespan& Duration) const
    {
        for (auto& Future : Futures)
        {
            bool bNotReady = false;
            if (!Future.IsReady())
            {
                if (!Future.WaitFor(Duration)) return false;
            }
        }

        return true;
    }

private:
    TArray<TFuture<void>> Futures;
};

/**
 *  Synchronous parallel execution of work. Occupies all available threads
 *  @param NumWorks Amount of work that will be allocated for execution
 *  @param Callable Function that will be called on the thread. The transmitted index acts as an index of the work assigned to it
*/
GLT_API FMultiFutureHandle ParallelDoWork(int32 NumWorks, TFunction<void(int32 WorkerIndex, int32 WorkIndex)>&& Callable);