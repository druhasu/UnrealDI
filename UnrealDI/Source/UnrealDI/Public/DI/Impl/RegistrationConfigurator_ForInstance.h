// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "DI/Impl/RegistrationConfiguratorBase.h"
#include "DI/Impl/Operations/AsOperation.h"
#include "DI/Impl/Operations/AsSelfOperation.h"
#include "DI/Impl/Operations/ByInterfacesOperation.h"
#include "DI/Impl/Lifetimes.h"

namespace UnrealDI_Impl
{
    template<typename TObject>
    class TRegistrationConfigurator_ForInstance
        : public FRegistrationConfiguratorBase
        , public RegistrationOperations::AsOperation< TRegistrationConfigurator_ForInstance<TObject> >
        , public RegistrationOperations::AsSelfOperation< TRegistrationConfigurator_ForInstance<TObject> >
        , public RegistrationOperations::ByInterfacesOperation< TRegistrationConfigurator_ForInstance<TObject> >
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

        TSharedRef<FLifetimeHandler> GetLifetimeHandler() const override
        {
            return MakeShared<UnrealDI_Impl::FLifetimeHandler_Instance>(Instance);
        }

    private:
        friend class RegistrationOperations::AsOperation< TRegistrationConfigurator_ForInstance<TObject> >;
        friend class RegistrationOperations::AsSelfOperation< TRegistrationConfigurator_ForInstance<TObject> >;
        friend class RegistrationOperations::ByInterfacesOperation< TRegistrationConfigurator_ForInstance<TObject> >;

        TObject* Instance;
    };
}