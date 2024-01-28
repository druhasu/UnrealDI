// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "Containers/Map.h"
#include "DI/Impl/InstanceInjector.h"
#include "DI/Impl/InstanceConstructor.h"

class UObject;
class UClass;

namespace UnrealDI_Impl
{
    class FRegistrationStorage;

    class UNREALDI_API FDependenciesRegistry
    {
    public:
        using FConstructFunctionPtr = UObject* (*)(UWorld* World, UObject* Outer, UClass* EffectiveClass);
        using FInitFunctionPtr = void (*)(UObject& ConstructedObject, FRegistrationStorage& Container);
        using FPostInitFunctionPtr = void (*)(UObject* ConstructedObject);

        template <typename T>
        static void ExposeDependencies()
        {
            FUnprocessedEntry& Entry = GetUnprocessedEntries().Emplace_GetRef();
            Entry.ClassGetter = &T::StaticClass;
            Entry.ConstructFunction = &TInstanceConstructor<T>::Construct;
            Entry.InitFunction = &TInstanceInjector<T>::Invoke;
            Entry.PostInitFunction = &TInstanceConstructor<T>::PostInit;
        }

        static void ProcessPendingRegistrations();

        static FInitFunctionPtr FindInitFunction(UClass* Class);

    private:
        using FClassGetter = UClass* (*)();

        struct FUnprocessedEntry
        {
            FClassGetter ClassGetter;
            FConstructFunctionPtr ConstructFunction;
            FInitFunctionPtr InitFunction;
            FPostInitFunctionPtr PostInitFunction;
        };

        static TArray<FUnprocessedEntry>& GetUnprocessedEntries();

        static TMap<UClass*, FInitFunctionPtr> InitFunctions;
    };
}