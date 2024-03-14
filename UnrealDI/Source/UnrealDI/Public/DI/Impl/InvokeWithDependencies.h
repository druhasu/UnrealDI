// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "DI/Impl/DependencyResolverInvoker.h"

namespace UnrealDI_Impl
{
    template <typename... TArgs>
    struct TFunctionWithDependenciesInvoker
    {
        /* Invokes given function with args injected from Container */
        template <typename TFunction>
        static void Invoke(FRegistrationStorage& Resolver, TFunction&& Function)
        {
            Function(UnrealDI_Impl::TDependencyResolverInvoker<TArgs>(Resolver)...);
        }
    };

    /* Provides type of TFunctionWithDependenciesInvoker based on TFunction arguments */
    template <typename TFunction>
    struct TFunctionWithDependenciesInvokerProvider : public TFunctionWithDependenciesInvokerProvider< decltype(&TFunction::operator()) >
    {
    };

    /* Specialization for member function */
    template <typename TClass, typename TResult, typename... TArgs>
    struct TFunctionWithDependenciesInvokerProvider<TResult(TClass::*)(TArgs...) const>
    {
        using Invoker = TFunctionWithDependenciesInvoker<TArgs...>;
    };

}
