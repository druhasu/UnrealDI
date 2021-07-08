// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "Templates/UnrealTypeTraits.h"

namespace UnrealDI_Impl
{
namespace RegistrationOperations
{
    template<typename TConfigurator>
    class TAsSelfOperation
    {
    public:
        /* Registers type to be resolvable as itself */
        TConfigurator& AsSelf()
        {
            TConfigurator& This = StaticCast<TConfigurator&>(*this);
            This.InterfaceTypes.AddUnique(TConfigurator::ImplType::StaticClass());

            return This;
        }
    };
}
}