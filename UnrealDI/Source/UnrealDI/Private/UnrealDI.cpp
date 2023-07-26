// Copyright Andrei Sudarikov. All Rights Reserved.

#include "Modules/ModuleManager.h"
#include "DI/Impl/DependenciesRegistry.h"

class FUnrealDIModuleImpl : public IModuleInterface
{
public:
    void StartupModule() override
    {
        FModuleManager::Get().OnModulesChanged().AddRaw(this, &FUnrealDIModuleImpl::RegisterDependencies);
        UnrealDI_Impl::FDependenciesRegistry::ProcessPendingRegistrations();
    }

    void ShutdownModule() override
    {
        FModuleManager::Get().OnModulesChanged().RemoveAll(this);
    }

private:
    void RegisterDependencies(FName InModule, EModuleChangeReason InReason)
    {
        if (InReason == EModuleChangeReason::ModuleLoaded)
        {
            UnrealDI_Impl::FDependenciesRegistry::ProcessPendingRegistrations();
        }
    }
};

IMPLEMENT_MODULE(FUnrealDIModuleImpl, UnrealDI)