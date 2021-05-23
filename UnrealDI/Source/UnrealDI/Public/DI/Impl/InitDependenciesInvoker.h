// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "DI/Impl/ArgumentPack.h"
#include "DI/Impl/HasInitDependencies.h"

namespace UnrealDI_Impl
{
    class FRegistrationStorage;

    // helper struct to call InitDependencies with specified arguments
    template <typename T, typename TArgumentPack>
    struct TInitDependenciesInvoker;

    // specialization for classes without arguments
    template <typename T>
    struct TInitDependenciesInvoker<T, TArgumentPack<>>
    {
        static void Invoke(T* Self, FRegistrationStorage& Resolver);
    };

    // specialization for classes with arguments
    template <typename T, typename... TArgs>
    struct TInitDependenciesInvoker<T, TArgumentPack<TArgs...>>
    {
        static void Invoke(T* Self, FRegistrationStorage& Resolver);
    };


    // helper struct to decide whether we need to call InitDependencies without arguments
    template <typename T, typename = void>
    struct TInitDependeciesNoArgsInvoker;
    
    // specialization for classes that have InitDependencies()
    template <typename T>
    struct TInitDependeciesNoArgsInvoker<T, typename TEnableIf< THasInitDependencies<T>::Value >::Type>
    {
        static void Invoke(T* Self) { Self->InitDependencies(); }
    };

    // specialization for classes that have no InitDependencies
    template <typename T>
    struct TInitDependeciesNoArgsInvoker<T, typename TEnableIf< THasNoInitDependencies<T>::Value >::Type>
    {
        static void Invoke(T* Self) { /* call nothing, class have no InitDependencies method */ }
    };
}

#include "DI/Impl/DependencyResolverInvoker.h"

template<typename T>
void UnrealDI_Impl::TInitDependenciesInvoker<T, UnrealDI_Impl::TArgumentPack<>>::Invoke(T* Self, FRegistrationStorage& Resolver)
{
    UnrealDI_Impl::TInitDependeciesNoArgsInvoker<T>::Invoke(Self);
}

template <typename T, typename... TArgs>
void UnrealDI_Impl::TInitDependenciesInvoker<T, UnrealDI_Impl::TArgumentPack<TArgs...>>::Invoke(T* Self, FRegistrationStorage& Resolver)
{
    Self->InitDependencies(TDependencyResolverInvoker<typename TArgs::Type>(Resolver)...);
}