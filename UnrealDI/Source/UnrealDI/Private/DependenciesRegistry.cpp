// Copyright Andrei Sudarikov. All Rights Reserved.

#include "DI/Impl/DependenciesRegistry.h"

void UnrealDI_Impl::FDependenciesRegistry::Init()
{
    PostGarbageCollectHandle = FCoreUObjectDelegates::GetPostGarbageCollect().AddStatic(&FDependenciesRegistry::PostGarbageCollect);
}

void UnrealDI_Impl::FDependenciesRegistry::Shutdown()
{
    FCoreUObjectDelegates::GetPostGarbageCollect().Remove(PostGarbageCollectHandle);
}

void UnrealDI_Impl::FDependenciesRegistry::ProcessPendingRegistrations()
{
    if (GIsInitialLoad)
    {
        // wait until UObject subsystem is loaded
        return;
    }

    TArray<FUnprocessedEntry>& UnprocessedEntries = GetUnprocessedEntries();
    if (UnprocessedEntries.Num() > 0)
    {
        for (const FUnprocessedEntry& Entry : UnprocessedEntries)
        {
            NativeInitFunctions.Emplace(Entry.ClassGetter(), Entry.InitFunction);
        }

        UnprocessedEntries.Empty();
    }
}

void UnrealDI_Impl::FDependenciesRegistry::ClearBlueprintInitFunctionsCache()
{
    for (auto It = CachedInitFunctions.CreateIterator(); It; ++It)
    {
        UClass* Class = It.Key().Get();
        if (Class == nullptr)
        {
            It.RemoveCurrent();
            continue;
        }

        if (!Class->IsNative())
        {
            It.RemoveCurrent();
        }
    }
}

void UnrealDI_Impl::FDependenciesRegistry::FindInitFunctions(UClass* Class, FInitFunctionPtr& OutNativeInitFunction, UFunction*& OutBlueprintInitFunction)
{
    // check cache first
    FCacheEntry* CacheEntry = CachedInitFunctions.Find(Class);

    if (!CacheEntry)
    {
        CacheEntry = AddInitFunctionsToCache(Class);
    }

    OutNativeInitFunction = CacheEntry->NativeInitFunction;
    OutBlueprintInitFunction = CacheEntry->BlueprintInitFunction;
}

FName UnrealDI_Impl::FDependenciesRegistry::MakeInitDependenciesFunctionName(UClass* Class)
{
    return FName(FString::Printf(TEXT("InitDependencies_%s"), *Class->GetName()));
}

TArray<UnrealDI_Impl::FDependenciesRegistry::FUnprocessedEntry>& UnrealDI_Impl::FDependenciesRegistry::GetUnprocessedEntries()
{
    static TArray<FUnprocessedEntry> Result;
    return Result;
}

UnrealDI_Impl::FDependenciesRegistry::FCacheEntry* UnrealDI_Impl::FDependenciesRegistry::AddInitFunctionsToCache(UClass* Class)
{
    UClass* ClassIterator = Class;
    FCacheEntry NewEntry;

    while ((!NewEntry.NativeInitFunction || !NewEntry.BlueprintInitFunction) && ClassIterator)
    {
        if (!ClassIterator->IsNative())
        {
            if (!NewEntry.BlueprintInitFunction)
            {
                FName FunctionName = MakeInitDependenciesFunctionName(ClassIterator);

                NewEntry.BlueprintInitFunction = ClassIterator->FindFunctionByName(FunctionName);
            }
        }
        else
        {
            if (!NewEntry.NativeInitFunction)
            {
                NewEntry.NativeInitFunction = NativeInitFunctions.FindRef(ClassIterator);
            }
        }

        ClassIterator = ClassIterator->GetSuperClass();
    }

    return &CachedInitFunctions.Add(Class, MoveTemp(NewEntry));
}

void UnrealDI_Impl::FDependenciesRegistry::PostGarbageCollect()
{
    for (auto It = CachedInitFunctions.CreateIterator(); It; ++It)
    {
        if (!It.Key().IsValid())
        {
            It.RemoveCurrent();
        }
    }
}
