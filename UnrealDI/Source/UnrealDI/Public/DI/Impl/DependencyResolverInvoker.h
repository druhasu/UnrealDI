// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "DI/Impl/DependencyResolver.h"
#include "DI/Impl/IsSupportedArgument.h"
#include "Templates/EnableIf.h"

namespace UnrealDI_Impl
{
    class FRegistrationStorage;

    /*
     * Invokes IResolverImpl with correct argument
     * Main purpose of this class is to provide conversion operator to be called with type of argument that InitDependencies requested
     */
    template <typename TDependency>
    struct TDependencyResolverInvoker
    {
        using TResultType = typename TDecay<TDependency>::Type;

        TDependencyResolverInvoker(FRegistrationStorage& Resolver)
            : Resolver(Resolver)
        {
        }

        // this conversion operator is called when this object is casted to a parameter of InitDependencies method
        /*template
        <
            typename TEnableIf
            <
                //!std::is_convertible< TParent, TDependency >::value &&
                !TIsSame<TDependency, UObject*>::Value && // disallow cast to UObject* so TScriptInterface won't try to construct directly from this object
                TIsSupportedArgument< TDependency >::Value,
                bool
            >::Type = true
        >*/
        operator TResultType()
        {
            return TDependencyResolver< TResultType >::Resolve(Resolver);
        }

        FRegistrationStorage& Resolver;
    };
}