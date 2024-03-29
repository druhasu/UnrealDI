// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "DI/Factory.h"
#include "DI/ObjectsCollection.h"
#include "DI/Impl/RegistrationStorage.h"
#include "DI/Impl/StaticClass.h"
#include "UObject/ScriptInterface.h"
#include "UObject/ObjectPtr.h"

namespace UnrealDI_Impl
{

    template <typename T>
    struct TDependencyResolver;

    template <typename T>
    struct TDependencyResolver< T* >
    {
        using ReturnType = T*;

        static ReturnType Resolve(const FRegistrationStorage& Resolver)
        {
            // call ResolveImpl here to leverage auto-registration of concrete types
            return Cast<T>(Resolver.Resolve(TStaticClass<T>::StaticClass()));
        }
    };

    template <typename T>
    struct TDependencyResolver< TObjectPtr<T> >
    {
        using ReturnType = TObjectPtr<T>;

        static ReturnType Resolve(const FRegistrationStorage& Resolver)
        {
            // call ResolveImpl here to leverage auto-registration of concrete types
            return Cast<T>(Resolver.Resolve(TStaticClass<T>::StaticClass()));
        }
    };

    template <typename T>
    struct TDependencyResolver< TScriptInterface<T> >
    {
        using ReturnType = TScriptInterface<T>;

        static ReturnType Resolve(const FRegistrationStorage& Resolver)
        {
            // call ResolveImpl here to leverage auto-registration of concrete types
            return Resolver.Resolve(TStaticClass<T>::StaticClass());
        }
    };

    template <typename T>
    struct TDependencyResolver< TObjectsCollection<T> >
    {
        using ReturnType = TObjectsCollection<T>;

        static ReturnType Resolve(const FRegistrationStorage& Resolver)
        {
            return Resolver.ResolveAll(TStaticClass<T>::StaticClass());
        }
    };

    template <typename T>
    struct TDependencyResolver< TFactory<T> >
    {
        using ReturnType = TFactory<T>;

        static ReturnType Resolve(const FRegistrationStorage& Resolver)
        {
            return TFactory<T>(Resolver);
        }
    };
}
