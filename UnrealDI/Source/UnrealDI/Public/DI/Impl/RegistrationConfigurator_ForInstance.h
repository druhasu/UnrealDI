// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "DI/Impl/RegistrationConfiguratorBase.h"
#include "DI/Impl/Operations/AsOperation.h"
#include "DI/Impl/Operations/AsSelfOperation.h"
#include "DI/Impl/Operations/ByInterfacesOperation.h"
#include "DI/Impl/Lifetimes.h"

namespace UnrealDI_Impl
{
#define ThisType TRegistrationConfigurator_ForInstance<TObject>

    template<typename TObject>
    class TRegistrationConfigurator_ForInstance
        : public FRegistrationConfiguratorBase
        , public RegistrationOperations::TAsOperation< ThisType >
        , public RegistrationOperations::TAsSelfOperation< ThisType >
        , public RegistrationOperations::TByInterfacesOperation< ThisType >
    {
    public:
        using ImplType = TObject;

        TRegistrationConfigurator_ForInstance(const TRegistrationConfigurator_ForInstance&) = delete;
        TRegistrationConfigurator_ForInstance(TRegistrationConfigurator_ForInstance&&) = default;

        TRegistrationConfigurator_ForInstance(TObject* Instance)
            : FRegistrationConfiguratorBase(TObject::StaticClass())
            , Instance(Instance)
        {
        }

    private:
        friend class RegistrationOperations::TAsOperation< ThisType >;
        friend class RegistrationOperations::TAsSelfOperation< ThisType >;
        friend class RegistrationOperations::TByInterfacesOperation< ThisType >;

        FLifetimeHandler* CreateLifetimeHandler() const override
        {
            return new UnrealDI_Impl::FLifetimeHandler_Instance(Instance);
        }

        TObject* Instance;
    };

#undef ThisType
}
