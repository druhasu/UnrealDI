// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "DI/ObjectContainer.h"
#include "DI/Impl/InitDependenciesInvoker.h"
#include "DI/Impl/InitMethodTypologyDeducer.h"

/*
 * Base class for objects that need injection from World-bound container when the object is constructed.
 * You must call TryInitDependencies(this) in constructor of derived class.
 * You also need to register container with you World using FInjectOnConstruction::SetContainerForWorld()
 */
class UNREALDI_API FInjectOnConstruction
{
public:
    /*
     * Assigns Container to World.
     */
    static void SetContainerForWorld(class UWorld* World, UObjectContainer* Container);

    /*
     * Clears container for given World.
     */
    static void ClearContainerForWorld(class UWorld* World);

    /*
     * Returns container of a given UWorld or nullptr if not found
     */
    static UObjectContainer* GetContainerForWorld(class UWorld* World)
    {
        check(World);
        return ContainerMap.FindRef(World);
    }

protected:
    /*
     * Call this method from your subclass constructor.
     */
    template <typename T>
    void TryInitDependencies(T* Self)
    {
        UWorld* World = Self->GetWorld();
        UObjectContainer** ContainerPtr = ContainerMap.Find(World);

        if (ContainerPtr)
        {
            using Invoker = UnrealDI_Impl::TInitDependenciesInvoker<T, UnrealDI_Impl::TInitMethodTypologyDeducer< T >>;

            Invoker::Invoke(Self, **ContainerPtr);
        }
    }

private:
    static TMap<UWorld*, UObjectContainer*> ContainerMap;
};
