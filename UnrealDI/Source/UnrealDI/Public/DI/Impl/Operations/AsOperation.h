// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "Templates/UnrealTypeTraits.h"

namespace UnrealDI_Impl
{
namespace RegistrationOperations
{
    template<typename TConfigurator>
    class AsOperation
    {
    public:
        template<typename TInterface>
        TConfigurator& As()
        {
            static_assert(TIsDerivedFrom<TConfigurator::ImplType, TInterface>::Value, "Implementation type must be derived from Interface type");

            TConfigurator& This = StaticCast<TConfigurator&>(*this);
            This.InterfaceTypes.AddUnique(TInterface::UClassType::StaticClass());

            return This;
        }
    };
}
}