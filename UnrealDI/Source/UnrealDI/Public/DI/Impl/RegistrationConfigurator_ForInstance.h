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
        , public RegistrationOperations::TAsOperation< TRegistrationConfigurator_ForInstance<TObject> >
        , public RegistrationOperations::TAsSelfOperation< TRegistrationConfigurator_ForInstance<TObject> >
        , public RegistrationOperations::TByInterfacesOperation< TRegistrationConfigurator_ForInstance<TObject> >
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

        TSharedRef<FLifetimeHandler> CreateLifetimeHandler() const override
        {
            return MakeShared<UnrealDI_Impl::FLifetimeHandler_Instance>(Instance);
        }

    private:
        friend class RegistrationOperations::TAsOperation< TRegistrationConfigurator_ForInstance<TObject> >;
        friend class RegistrationOperations::TAsSelfOperation< TRegistrationConfigurator_ForInstance<TObject> >;
        friend class RegistrationOperations::TByInterfacesOperation< TRegistrationConfigurator_ForInstance<TObject> >;

        TObject* Instance;
    };
}