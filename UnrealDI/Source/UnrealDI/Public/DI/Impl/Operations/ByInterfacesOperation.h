// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "Templates/UnrealTypeTraits.h"

namespace UnrealDI_Impl
{
namespace RegistrationOperations
{
    template<typename TConfigurator>
    class TByInterfacesOperation
    {
    public:
        TConfigurator& ByInterfaces()
        {
            TConfigurator& This = StaticCast<TConfigurator&>(*this);
            UClass* ImplClass = This.ImplClass;
            for (auto& Interface : ImplClass->Interfaces)
            {
                This.InterfaceTypes.AddUnique(Interface.Class);
            }

            return This;
        }
    };
}
}