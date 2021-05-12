// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "DI/Impl/RegistrationConfiguratorBase.h"
#include "DI/Impl/Operations/AsOperation.h"
#include "DI/Impl/Operations/AsSelfOperation.h"
#include "DI/Impl/Operations/ByInterfacesOperation.h"
#include "DI/Impl/Operations/SingleInstanceOperation.h"
#include "DI/Impl/Operations/WeakSingleInstanceOperation.h"
#include "DI/Impl/Lifetimes.h"

class IResolver;

namespace UnrealDI_Impl
{
#define ThisType TRegistrationConfigurator_ForFactory<TObject>

    template<typename TObject>
    class TRegistrationConfigurator_ForFactory
        : public FRegistrationConfiguratorBase
        , public RegistrationOperations::TAsOperation< ThisType >
        , public RegistrationOperations::TAsSelfOperation< ThisType >
        , public RegistrationOperations::TByInterfacesOperation< ThisType >
        , public RegistrationOperations::TSingleInstanceOperation< ThisType >
        , public RegistrationOperations::TWeakSingleInstanceOperation< ThisType >
    {
    public:
        using ImplType = TObject;

        TRegistrationConfigurator_ForFactory(const TRegistrationConfigurator_ForFactory&) = delete;
        TRegistrationConfigurator_ForFactory(TRegistrationConfigurator_ForFactory&&) = default;

        TRegistrationConfigurator_ForFactory(TFunction< TObject* () > Factory)
            : FRegistrationConfiguratorBase(TObject::StaticClass())
            , Factory(Factory)
        {
        }

        TSharedRef<FLifetimeHandler> CreateLifetimeHandler() const override
        {
            return MakeShared<UnrealDI_Impl::FLifetimeHandler_Transient>(GetFactory());
        }

    private:
        friend class RegistrationOperations::TAsOperation< ThisType >;
        friend class RegistrationOperations::TAsSelfOperation< ThisType >;
        friend class RegistrationOperations::TByInterfacesOperation< ThisType >;
        friend class RegistrationOperations::TSingleInstanceOperation< ThisType >;
        friend class RegistrationOperations::TWeakSingleInstanceOperation< ThisType >;

        TFunction< TObject* () > Factory;

        FInstanceFactoryResult GetFactory() const
        {
            // store Factory in a variable to capture only it, instead of this
            auto FactoryVar = Factory;
            return
            {
                [FactoryVar = MoveTemp(FactoryVar)] (FRegistrationStorage& Resolver)
                {
                    auto Ret = FactoryVar();
                    using Invoker = UnrealDI_Impl::TInitDependenciesInvoker<TObject, UnrealDI_Impl::TInitMethodTypologyDeducer< TObject >>;
                    Invoker::Invoke(Ret, Resolver);
                    return Ret;
                },
                nullptr // we don't need to keep any additional class alive
            };
        }
    };

#undef ThisType
}