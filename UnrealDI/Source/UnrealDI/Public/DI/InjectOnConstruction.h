// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "Containers/Map.h"

class UWorld;
class UObjectContainer;

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
    static void SetContainerForWorld(UWorld* World, UObjectContainer* Container);

    /*
     * Clears container for given World.
     */
    static void ClearContainerForWorld(UWorld* World);

    /*
     * Returns container of a given UWorld or nullptr if not found
     */
    static UObjectContainer* GetContainerForWorld(UWorld* World);

protected:
    friend class UUnrealDIBlueprintLibrary; // needs access to TryInitDependenciesInternal

    /*
     * Call this method from your subclass constructor.
     */
    void TryInitDependencies(UObject* Self);

private:
    static void TryInitDependenciesInternal(UObject* Self);
};
