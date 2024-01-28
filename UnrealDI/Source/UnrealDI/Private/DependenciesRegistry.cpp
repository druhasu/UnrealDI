// Copyright Andrei Sudarikov. All Rights Reserved.

#include "DI/Impl/DependenciesRegistry.h"

TMap<UClass*, UnrealDI_Impl::FDependenciesRegistry::FInitFunctionPtr> UnrealDI_Impl::FDependenciesRegistry::InitFunctions;

void UnrealDI_Impl::FDependenciesRegistry::ProcessPendingRegistrations()
{
    if (GIsInitialLoad)
    {
        // wait until UObject subsystem is loaded
        return;
    }

    if (GetUnprocessedEntries().Num() > 0)
    {
        for (const FUnprocessedEntry& Entry : GetUnprocessedEntries())
        {
            InitFunctions.Emplace(Entry.ClassGetter(), Entry.InitFunction);
        }

        GetUnprocessedEntries().Empty();
    }
}

UnrealDI_Impl::FDependenciesRegistry::FInitFunctionPtr UnrealDI_Impl::FDependenciesRegistry::FindInitFunction(UClass* Class)
{
    // find native parent, because we store only those in InitFunctions
    while (!Class->IsNative())
    {
        Class = Class->GetSuperClass();
    }

    // try find an InitFunction for a class
    FInitFunctionPtr Result = nullptr;
    while (!Result && Class)
    {
        Result = InitFunctions.FindRef(Class);
        Class = Class->GetSuperClass();
    }

    return Result;
}

TArray<UnrealDI_Impl::FDependenciesRegistry::FUnprocessedEntry>& UnrealDI_Impl::FDependenciesRegistry::GetUnprocessedEntries()
{
    static TArray<FUnprocessedEntry> Result;
    return Result;
}
