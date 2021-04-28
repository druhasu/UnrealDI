// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "Containers/Array.h"
#include "Templates/SharedPointer.h"

class UClass;
class FObjectContainerBuilder;

namespace UnrealDI_Impl
{
    class FLifetimeHandler;

    class FRegistrationConfiguratorBase
    {
    public:
        FRegistrationConfiguratorBase(UClass* InType)
            : ImplClass(InType)
        {
        }

        virtual ~FRegistrationConfiguratorBase() = default;
        virtual TSharedRef<FLifetimeHandler> CreateLifetimeHandler() const = 0;

    protected:
        friend class ::FObjectContainerBuilder;

        UClass* ImplClass;
        TArray<UClass*> InterfaceTypes;
    };
}