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
        , public RegistrationOperations::TAsOperation< ThisType >
        , public RegistrationOperations::TAsSelfOperation< ThisType >
        , public RegistrationOperations::TByInterfacesOperation< ThisType >
        , public RegistrationOperations::TSingleInstanceOperation< ThisType >
        , public RegistrationOperations::TWeakSingleInstanceOperation< ThisType >
        , public RegistrationOperations::TFromBlueprintOperation< ThisType, TObject >
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
            LifetimeHandlerFactory = &ThisType::CreateDefaultLifetimeHandler;
        }

        TSharedRef<FLifetimeHandler> CreateLifetimeHandler() const override
        {
            return LifetimeHandlerFactory(this);
        }

    private:
        friend class RegistrationOperations::TAsOperation< ThisType >;
        friend class RegistrationOperations::TAsSelfOperation< ThisType >;
        friend class RegistrationOperations::TByInterfacesOperation< ThisType >;
        friend class RegistrationOperations::TSingleInstanceOperation< ThisType >;
        friend class RegistrationOperations::TWeakSingleInstanceOperation< ThisType >;
        friend class RegistrationOperations::TFromBlueprintOperation< ThisType, TObject >;

        FInstanceFactoryResult GetFactory() const
        {
            return TInstanceFactory<TObject>::CreateFactory(EffectiveClass);
        }

        static TSharedRef<FLifetimeHandler> CreateDefaultLifetimeHandler(const ThisType* This)
        {
            return MakeShared<UnrealDI_Impl::FLifetimeHandler_Transient>(This->GetFactory());
        }

        UClass* EffectiveClass;
        FLifetimeHandlerFactory LifetimeHandlerFactory;
    };

#undef ThisType
}