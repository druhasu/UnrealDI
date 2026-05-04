// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "Containers/Array.h"
#include "UObject/SoftObjectPtr.h"

class UClass;
class UObject;
class FObjectContainerBuilder;

namespace UnrealDI_Impl
{
    class FLifetimeHandler;

    class FRegistrationConfiguratorBase
    {
    public:
        FRegistrationConfiguratorBase(UClass* InType)
            : ImplClass(InType), EffectiveClassPtr(InType)
        {
        }

        virtual ~FRegistrationConfiguratorBase() = default;
        virtual FLifetimeHandler* CreateLifetimeHandler() const = 0;

    protected:
        friend class ::FObjectContainerBuilder;

        UClass* ImplClass;
        TArray<UClass*> InterfaceTypes;
        TSoftClassPtr<UObject> EffectiveClassPtr;
        bool bAutoCreate = false;
    };
}
