// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "DI/Impl/RegistrationConfiguratorBase.h"
#include "DI/Impl/Operations/AsOperation.h"
#include "DI/Impl/Operations/AsSelfOperation.h"
#include "DI/Impl/Operations/ByInterfacesOperation.h"
#include "DI/Impl/Operations/SingleInstanceOperation.h"
#include "DI/Impl/Operations/WeakSingleInstanceOperation.h"
#include "DI/Impl/Operations/FromBlueprintOperation.h"
#include "UObject/Interface.h"
#include "Templates/UnrealTypeTraits.h"

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
        // warn user if he tries to register UInterface boilerplate class instead of actual implementation
        static_assert(!TIsDerivedFrom<TObject, UInterface>::Value, "You are trying to register UInterface derived class. This is probably a typo");

        using ImplType = TObject;
        using FLifetimeHandlerFactory = TSharedRef<FLifetimeHandler>(*)();

        TRegistrationConfigurator_ForType(const TRegistrationConfigurator_ForType&) = delete;
        TRegistrationConfigurator_ForType(TRegistrationConfigurator_ForType&&) = default;

        TRegistrationConfigurator_ForType()
            : TRegistrationConfigurator_ForType(TObject::StaticClass())
        {
        }

        TRegistrationConfigurator_ForType(UClass* InType)
            : FRegistrationConfiguratorBase(InType)
        {
            LifetimeHandlerFactory = &FLifetimeHandler_Transient::Make;
        }

        TSharedRef<FLifetimeHandler> CreateLifetimeHandler() const override
        {
            return LifetimeHandlerFactory();
        }

    private:
        friend class RegistrationOperations::TAsOperation< ThisType >;
        friend class RegistrationOperations::TAsSelfOperation< ThisType >;
        friend class RegistrationOperations::TByInterfacesOperation< ThisType >;
        friend class RegistrationOperations::TSingleInstanceOperation< ThisType >;
        friend class RegistrationOperations::TWeakSingleInstanceOperation< ThisType >;
        friend class RegistrationOperations::TFromBlueprintOperation< ThisType, TObject >;

        FLifetimeHandlerFactory LifetimeHandlerFactory;
    };

#undef ThisType
}
