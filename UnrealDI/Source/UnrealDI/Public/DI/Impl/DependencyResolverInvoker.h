// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "DI/DependencyResolver.h"

namespace UnrealDI_Impl
{
    /*
     * Invokes IResolverImpl with correct argument
     * Main purpose of this class is to provide conversion operator to be called with type of argument that InitDependencies requested
     */
    template <typename TDependency>
    struct TDependencyResolverInvoker
    {
        using TResultType = typename TDecay<TDependency>::Type;

        TDependencyResolverInvoker(const IResolver& Resolver)
            : Resolver(Resolver)
        {
        }

        // this conversion operator is called when this object is casted to a parameter of InitDependencies method
        operator TResultType()
        {
            return TDependencyResolver< TResultType >::Resolve(Resolver);
        }

        const IResolver& Resolver;
    };
}
