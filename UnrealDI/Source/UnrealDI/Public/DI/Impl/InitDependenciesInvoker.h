// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "DI/Impl/ArgumentPack.h"
#include "DI/Impl/DependencyResolverInvoker.h"

namespace UnrealDI_Impl
{
    template <typename T, typename TArgumentPack>
    struct TInitDependenciesInvoker;

    template <typename T>
    struct TInitDependenciesInvoker<T, TArgumentPack<>>
    {
        static void Invoke(T* Self, UObjectContainer& Resolver)
        {
        }
    };

    template <typename T, typename... TArgs>
    struct TInitDependenciesInvoker<T, TArgumentPack<TArgs...>>
    {
        static void Invoke(T* Self, UObjectContainer& Resolver)
        {
            Self->InitDependencies(TDependencyResolverInvoker<typename TArgs::Type>(Resolver)...);
        }
    };
}