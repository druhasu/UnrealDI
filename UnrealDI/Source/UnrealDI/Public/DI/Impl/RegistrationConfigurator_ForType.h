// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "DI/Impl/RegistrationConfiguratorBase.h"
#include "DI/Impl/Operations/AsOperation.h"
#include "DI/Impl/Operations/AsSelfOperation.h"
#include "DI/Impl/Operations/ByInterfacesOperation.h"
#include "DI/Impl/Operations/SingleInstanceOperation.h"
#include "DI/Impl/Operations/WeakSingleInstanceOperation.h"
#include "DI/Impl/Operations/FromBlueprintOperation.h"
#include "DI/Impl/InstanceFactory.h"

namespace UnrealDI_Impl
{
#define ThisType TRegistrationConfigurator_ForType<TObject>

    template<typename TObject>
    class TRegistrationConfigurator_ForType
        : public FRegistrationConfiguratorBase
        , public RegistrationOperations::AsOperation< ThisType >
        , public RegistrationOperations::AsSelfOperation< ThisType >
        , public RegistrationOperations::ByInterfacesOperation< ThisType >
        , public RegistrationOperations::SingleInstanceOperation< ThisType >
        , public RegistrationOperations::WeakSingleInstanceOperation< ThisType >
        , public RegistrationOperations::FromBlueprintOperation< ThisType, TObject >
    {
    public:
        using ImplType = TObject;
        using FLifetimeHandlerFactory = TSharedRef<FLifetimeHandler>(*)(const ThisType*);

        TRegistrationConfigurator_ForType(const TRegistrationConfigurator_ForType&) = delete;
        TRegistrationConfigurator_ForType(TRegistrationConfigurator_ForType&&) = default;

        TRegistrationConfigurator_ForType()
            : FRegistrationConfiguratorBase(TObject::StaticClass())
            , EffectiveClass(TObject::StaticClass())
        {
            CreateLifetimeHandler = &ThisType::CreateDefaultLifetimeHandler;
        }

        TSharedRef<FLifetimeHandler> GetLifetimeHandler() const override
        {
            return CreateLifetimeHandler(this);
        }

    private:
        friend class RegistrationOperations::AsOperation< ThisType >;
        friend class RegistrationOperations::AsSelfOperation< ThisType >;
        friend class RegistrationOperations::ByInterfacesOperation< ThisType >;
        friend class RegistrationOperations::SingleInstanceOperation< ThisType >;
        friend class RegistrationOperations::WeakSingleInstanceOperation< ThisType >;
        friend class RegistrationOperations::FromBlueprintOperation< ThisType, TObject >;

        FInstanceFactoryCallable GetFactory() const
        {
            return TInstanceFactory<TObject>::CreateFactory(EffectiveClass);
        }

        static TSharedRef<FLifetimeHandler> CreateDefaultLifetimeHandler(const ThisType* This)
        {
            return MakeShared<UnrealDI_Impl::FLifetimeHandler_Transient>(This->GetFactory());
        }

        UClass* EffectiveClass;
        FLifetimeHandlerFactory CreateLifetimeHandler;
    };

#undef ThisType
}