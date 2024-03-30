// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "DI/Impl/StaticClass.h"

namespace UnrealDI_Impl
{
namespace RegistrationOperations
{
    template<typename TConfigurator>
    class TAsOperation
    {
    public:
        /* Registers type as to be resolvable as TInterface */
        template<typename TInterface>
        TConfigurator& As()
        {
            static_assert(TIsDerivedFrom<typename TConfigurator::ImplType, TInterface>::Value, "Implementation type must be derived from Interface type");

            TConfigurator& This = StaticCast<TConfigurator&>(*this);
            This.InterfaceTypes.AddUnique(TStaticClass<TInterface>::StaticClass());

            return This;
        }
    };
}
}