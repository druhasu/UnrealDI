// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "DI/Impl/RegistrationConfiguratorBase.h"
#include "DI/Impl/Operations/AsOperation.h"
#include "DI/Impl/Operations/AsSelfOperation.h"
#include "DI/Impl/Operations/ByInterfacesOperation.h"
#include "DI/Impl/Lifetimes.h"

namespace UnrealDI_Impl
{
#define ThisType TRegistrationConfigurator_ForCDO<TObject>

    template<typename TObject>
    class TRegistrationConfigurator_ForCDO
        : public FRegistrationConfiguratorBase
        , public RegistrationOperations::TAsOperation< ThisType >
        , public RegistrationOperations::TAsSelfOperation< ThisType >
        , public RegistrationOperations::TByInterfacesOperation< ThisType >
    {
    public:
        using ImplType = TObject;

        TRegistrationConfigurator_ForCDO(const TRegistrationConfigurator_ForCDO&) = delete;
        TRegistrationConfigurator_ForCDO(TRegistrationConfigurator_ForCDO&&) = default;

        TRegistrationConfigurator_ForCDO()
            : FRegistrationConfiguratorBase(TObject::StaticClass())
        {
        }

        TSharedRef<FLifetimeHandler> CreateLifetimeHandler() const override
        {
            return MakeShared<UnrealDI_Impl::FLifetimeHandler_Transient>(FInstanceFactoryInvoker{ &ThisType::GetDefaultInstance, nullptr });
        }

    private:
        friend class RegistrationOperations::TAsOperation< ThisType >;
        friend class RegistrationOperations::TAsSelfOperation< ThisType >;
        friend class RegistrationOperations::TByInterfacesOperation< ThisType >;

        static UObject* GetDefaultInstance(FRegistrationStorage&, UClass*)
        {
            return GetMutableDefault<TObject>();
        }
    };

#undef ThisType
}