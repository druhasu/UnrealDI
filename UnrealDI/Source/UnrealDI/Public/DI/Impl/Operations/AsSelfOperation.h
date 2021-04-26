// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "Templates/UnrealTypeTraits.h"

namespace UnrealDI_Impl
{
namespace RegistrationOperations
{
    template<typename TConfigurator>
    class AsSelfOperation
    {
    public:
        TConfigurator& AsSelf()
        {
            TConfigurator& This = StaticCast<TConfigurator&>(*this);
            This.InterfaceTypes.AddUnique(TConfigurator::ImplType::StaticClass());

            return This;
        }
    };
}
}