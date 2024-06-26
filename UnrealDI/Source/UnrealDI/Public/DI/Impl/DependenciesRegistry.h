// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "Containers/Map.h"

class UObject;
class UClass;
class IResolver;
class UFunction;

namespace UnrealDI_Impl
{
    class UNREALDI_API FDependenciesRegistry
    {
    public:
        using FInitFunctionPtr = void (*)(UObject& ConstructedObject, const IResolver& Container);

        static void Init();
        static void Shutdown();

        template <typename T>
        static void ExposeDependencies();

        static void ProcessPendingRegistrations();

        static void FindInitFunctions(UClass* Class, FInitFunctionPtr& OutNativeInitFunction, UFunction*& OutBlueprintInitFunction);

        static FName MakeInitDependenciesFunctionName(UClass* Class);

    private:
        using FClassGetter = UClass* (*)();

        struct FUnprocessedEntry
        {
            FClassGetter ClassGetter;
            FInitFunctionPtr InitFunction;
        };

        struct FCacheEntry
        {
            FInitFunctionPtr NativeInitFunction = nullptr;
            UFunction* BlueprintInitFunction = nullptr;
        };

        static TArray<FUnprocessedEntry>& GetUnprocessedEntries();
        static FCacheEntry* AddInitFunctionsToCache(UClass* Class);
        static void PostGarbageCollect();

        static inline TMap<UClass*, FInitFunctionPtr> NativeInitFunctions;
        static inline TMap<TWeakObjectPtr<UClass>, FCacheEntry> CachedInitFunctions;
        static inline FDelegateHandle PostGarbageCollectHandle;
    };
}

#include "DI/Impl/InstanceInjector.h"

template <typename T>
void UnrealDI_Impl::FDependenciesRegistry::ExposeDependencies()
{
    TArray<FUnprocessedEntry>& UnprocessedEntries = GetUnprocessedEntries();

    FUnprocessedEntry& Entry = UnprocessedEntries.Emplace_GetRef();
    Entry.ClassGetter = &T::StaticClass;
    Entry.InitFunction = &TInstanceInjector<T>::Invoke;
}
