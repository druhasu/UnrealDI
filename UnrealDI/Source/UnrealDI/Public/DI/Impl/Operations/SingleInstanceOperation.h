// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "Templates/UnrealTypeTraits.h"
#include "DI/Impl/Lifetimes.h"

namespace UnrealDI_Impl
{
namespace RegistrationOperations
{
    template<typename TConfigurator>
    class TSingleInstanceOperation
    {
    public:
        /* Only one instance will be created. Container will keep strong reference to this instance */
        TConfigurator& SingleInstance(bool bAutoCreate = false)
        {
            TConfigurator& This = StaticCast<TConfigurator&>(*this);
            This.LifetimeHandlerFactory = &FLifetimeHandler_SingleInstance::Make;
            This.bAutoCreate = bAutoCreate;

            return This;
        }
    };
}
}
