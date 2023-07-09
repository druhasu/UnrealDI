// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "UObject/Class.h"
#include "Templates/UnrealTypeTraits.h"

namespace UnrealDI_Impl
{
namespace RegistrationOperations
{
    template<typename TConfigurator>
    class TByInterfacesOperation
    {
    public:
        /* Registers type to be resolvable as all intefaces that it implements */
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