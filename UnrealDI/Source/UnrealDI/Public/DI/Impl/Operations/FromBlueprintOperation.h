// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "UObject/Class.h"

namespace UnrealDI_Impl
{
namespace RegistrationOperations
{
    template<typename TConfigurator, typename TImpl>
    class TFromBlueprintOperation
    {
    public:
        TConfigurator& FromBlueprint(TSubclassOf<TImpl> Class)
        {
            check(Class.Get());

            TConfigurator& This = StaticCast<TConfigurator&>(*this);
            This.EffectiveClass = Class;

            return This;
        }
    };
}
}