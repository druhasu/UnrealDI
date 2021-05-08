// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "DI/Impl/ArgumentPack.h"

namespace UnrealDI_Impl
{
    class FRegistrationStorage;

    template <typename T, typename TArgumentPack>
    struct TInitDependenciesInvoker;

    template <typename T>
    struct TInitDependenciesInvoker<T, TArgumentPack<>>
    {
        static void Invoke(T* Self, FRegistrationStorage& Resolver) { }
    };

    template <typename T, typename... TArgs>
    struct TInitDependenciesInvoker<T, TArgumentPack<TArgs...>>
    {
        static void Invoke(T* Self, FRegistrationStorage& Resolver);
    };
}

#include "DI/Impl/DependencyResolverInvoker.h"

template <typename T, typename... TArgs>
void UnrealDI_Impl::TInitDependenciesInvoker<T, UnrealDI_Impl::TArgumentPack<TArgs...>>::Invoke(T* Self, FRegistrationStorage& Resolver)
{
    Self->InitDependencies(TDependencyResolverInvoker<typename TArgs::Type>(Resolver)...);
}