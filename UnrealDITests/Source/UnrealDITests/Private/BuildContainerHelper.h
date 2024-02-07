// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "Templates/Function.h"

class UObject;
class UObjectContainer;
class FObjectContainerBuilder;

/*
 * Helper class for building container with commonly used types
 * There are some test specs where we need a container, but not the includes of classes that are inside it
 * In real world projects container configuration are always in separate .cpp file from the types it create
 * With this helper we try to emulate same conditions
 */
struct FBuildContainerHelper
{
    static UObjectContainer* Build(UObject* Outer, TFunctionRef<void(FObjectContainerBuilder&)> AddRegistrations);

    static UObjectContainer* Build(UObject* Outer)
    {
        return Build(Outer, [](FObjectContainerBuilder&) {});
    }

    static UObjectContainer* Build(TFunctionRef<void(FObjectContainerBuilder&)> AddRegistrations)
    {
        return Build(nullptr, AddRegistrations);
    }

    static UObjectContainer* Build()
    {
        return Build(nullptr, [](FObjectContainerBuilder&) {});
    }
};