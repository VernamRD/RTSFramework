#include "GLT.h"

#include "Core/GLTCore.h"

#define LOCTEXT_NAMESPACE "FGLTModule"

void FGLTModule::StartupModule()
{
    GLT::IGLT::Initialize();
}

void FGLTModule::ShutdownModule()
{
    GLT::IGLT::Deinitialize();
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FGLTModule, GLT)