#include "GLT.h"

#include "Core/GLThreadInitializer.h"

#define LOCTEXT_NAMESPACE "FGLTModule"

void FGLTModule::StartupModule()
{
    FGLThreadInitializer::Initialize();
}

void FGLTModule::ShutdownModule()
{
    FGLThreadInitializer::Deinitialize();
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FGLTModule, GLT)