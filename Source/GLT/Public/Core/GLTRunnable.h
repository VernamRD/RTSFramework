// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class GLT_API FGLTRunnable : public FRunnable
{
public:
    FGLTRunnable();
    virtual ~FGLTRunnable();

    virtual bool Init() override;
    virtual uint32 Run() override;
    virtual void Stop() override;
    bool IsStopped() const { return StopThreadCounter > 0; }

    float GetGLTToGTRatio() const { return GLTToGTCycleRatio; }

    virtual void Exit() override;

private:
    FEvent* EndFrameEvent;
    void OnEndFrame();
    
    std::atomic<float> GLTToGTCycleRatio;
    
    std::atomic<float> GLTDeltaSeconds;
    
    std::atomic<int32> StopThreadCounter;
    
#if DO_CHECK
    static std::atomic<int8> Debug_ThreadCounter;
#endif
};
