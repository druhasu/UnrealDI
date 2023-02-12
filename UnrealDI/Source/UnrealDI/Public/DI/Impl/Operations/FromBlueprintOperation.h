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
        /* Specifies subclass that will be instantiated instead of registered type */
        TConfigurator& FromBlueprint(TSubclassOf<TImpl> Class)
        {
            check(Class.Get());

            TConfigurator& This = StaticCast<TConfigurator&>(*this);
            This.EffectiveClassPtr = Class.Get();

            return This;
        }

        /* Specifies subclass that will be instantiated instead of registered type */
        TConfigurator& FromBlueprint(const TSoftClassPtr<TImpl>& ClassPtr)
        {
            check(!ClassPtr.IsNull());

            TConfigurator& This = StaticCast<TConfigurator&>(*this);
            This.EffectiveClassPtr = ClassPtr.ToSoftObjectPath();

            return This;
        }
    };
}
}