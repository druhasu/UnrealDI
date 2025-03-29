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
        /* Registers type to be resolvable as TInterface */
        template<typename TInterface>
        TConfigurator& As()
        {
            if constexpr (!std::is_same_v<typename TConfigurator::ImplType, UObject>)
            {
                // if you hit this assert, make sure that type passed to RegisterType<> is actually derived from TInterface
                static_assert(TIsDerivedFrom<typename TConfigurator::ImplType, TInterface>::Value, "Implementation type must be derived from Interface type");

                TConfigurator& This = StaticCast<TConfigurator&>(*this);
                This.InterfaceTypes.AddUnique(TStaticClass<TInterface>::StaticClass());

                return This;
            }
            else
            {
                return As(TStaticClass<TInterface>::StaticClass());
            }
        }

        /* Registers type to be resolvable as given Class */
        TConfigurator& As(UClass* InClass)
        {
            check(InClass != nullptr);

            TConfigurator& This = StaticCast<TConfigurator&>(*this);

            check(This.ImplClass->IsChildOf(InClass) || This.ImplClass->ImplementsInterface(InClass));
            This.InterfaceTypes.AddUnique(InClass);

            return This;
        }
    };
}
}
