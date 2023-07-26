// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "Containers/Map.h"
#include "DI/Impl/InstanceInjector.h"

class UObject;
class UClass;

namespace UnrealDI_Impl
{
    class FRegistrationStorage;

    class UNREALDI_API FDependenciesRegistry
    {
    public:
        using FInitFunctionPtr = void (*)(UObject&, FRegistrationStorage&);

        template <typename T>
        static uint8 ExposeDependencies()
        {
            FUnprocessedEntry& Entry = UnprocessedEntries.Emplace_GetRef();
            Entry.ClassGetter = &T::StaticClass;
            Entry.InitFunction = &UnrealDI_Impl::TInstanceInjector<T>::Invoke;

            return 1;
        }

        static void ProcessPendingRegistrations();

        static FInitFunctionPtr FindInitFunction(UClass* Class);

    private:
        using FClassGetter = UClass* (*)();

        struct FUnprocessedEntry
        {
            FClassGetter ClassGetter;
            FInitFunctionPtr InitFunction;
        };

        static TMap<UClass*, FInitFunctionPtr> InitFunctions;
        static TArray<FUnprocessedEntry> UnprocessedEntries;
    };
}