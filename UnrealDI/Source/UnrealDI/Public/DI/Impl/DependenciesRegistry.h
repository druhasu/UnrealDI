// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "Containers/Map.h"

class UObject;
class UClass;

namespace UnrealDI_Impl
{
    class FRegistrationStorage;

    class UNREALDI_API FDependenciesRegistry
    {
    public:
        using FInitFunctionPtr = void (*)(UObject& ConstructedObject, const FRegistrationStorage& Container);

        template <typename T>
        static void ExposeDependencies();

        static void ProcessPendingRegistrations();

        static FInitFunctionPtr FindInitFunction(UClass* Class);

    private:
        using FClassGetter = UClass* (*)();

        struct FUnprocessedEntry
        {
            FClassGetter ClassGetter;
            FInitFunctionPtr InitFunction;
        };

        static TArray<FUnprocessedEntry>& GetUnprocessedEntries();

        static TMap<UClass*, FInitFunctionPtr> InitFunctions;
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
