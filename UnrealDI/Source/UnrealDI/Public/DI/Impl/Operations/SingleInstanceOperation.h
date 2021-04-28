// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "Templates/UnrealTypeTraits.h"
#include "DI/Impl/Lifetimes.h"

namespace UnrealDI_Impl
{
namespace RegistrationOperations
{
    template<typename TConfigurator>
    class SingleInstanceOperation
    {
    public:
        TConfigurator& SingleInstance()
        {
            TConfigurator& This = StaticCast<TConfigurator&>(*this);
            This.LifetimeHandlerFactory = &SingleInstanceOperation::CreateLifetimeHandler;

            return This;
        }

    private:
        static TSharedRef<FLifetimeHandler> CreateLifetimeHandler(const TConfigurator* This)
        {
            return MakeShared<UnrealDI_Impl::FLifetimeHandler_SingleInstance>(This->GetFactory());
        }
    };
}
}