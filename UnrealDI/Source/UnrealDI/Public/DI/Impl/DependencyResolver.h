// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "DI/Factory.h"
#include "DI/ObjectsCollection.h"
#include "DI/Impl/IsUInterface.h"
#include "DI/Impl/StaticClass.h"
#include "Templates/EnableIf.h"
#include "UObject/ScriptInterface.h"

namespace UnrealDI_Impl
{

    template <typename T>
    struct TDependencyResolver;

    template <typename T>
    struct TDependencyResolver< T* >
    {
        using ReturnType = T*;

        static ReturnType Resolve(UObjectContainer& Resolver)
        {
            // call ResolveImpl here to leverage auto-registration of concrete types
            return Cast<T>(Resolver.ResolveImpl<T>());
        }
    };

    template <typename T>
    struct TDependencyResolver< TScriptInterface<T> >
    {
        using ReturnType = TScriptInterface<T>;

        static ReturnType Resolve(UObjectContainer& Resolver)
        {
            // call ResolveImpl here to leverage auto-registration of concrete types
            return Resolver.ResolveImpl<T>();
        }
    };

    template <typename T>
    struct TDependencyResolver< TObjectsCollection<T> >
    {
        using ReturnType = TObjectsCollection<T>;

        static ReturnType Resolve(UObjectContainer& Resolver)
        {
            return Resolver.ResolveAll<T>();
        }
    };

    template <typename T>
    struct TDependencyResolver< TFactory<T> >
    {
        using ReturnType = TFactory<T>;

        static ReturnType Resolve(UObjectContainer& Resolver)
        {
            return TFactory<T>(Resolver);
        }
    };
}