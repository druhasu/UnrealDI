// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "Templates/UnrealTypeTraits.h"
#include "DI/Impl/Lifetimes.h"

namespace UnrealDI_Impl
{
namespace RegistrationOperations
{
    template<typename TConfigurator>
    class TWeakSingleInstanceOperation
    {
    public:
        TConfigurator& WeakSingleInstance()
        {
            TConfigurator& This = StaticCast<TConfigurator&>(*this);
            This.LifetimeHandlerFactory = &TWeakSingleInstanceOperation::CreateLifetimeHandler;

            return This;
        }

    private:
        static TSharedRef<FLifetimeHandler> CreateLifetimeHandler(const TConfigurator* This)
        {
            return MakeShared<UnrealDI_Impl::FLifetimeHandler_WeakSingleInstance>(This->GetFactory());
        }
    };
}
}