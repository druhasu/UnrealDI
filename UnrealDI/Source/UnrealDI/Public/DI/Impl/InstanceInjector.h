// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

class UObject;

namespace UnrealDI_Impl
{
    class FRegistrationStorage;

    /* Generator class for injector functions */
    template<typename TObject>
    struct TInstanceInjector
    {
        static void Invoke(UObject& TargetObject, const FRegistrationStorage& Resolver);
    };
}

#include "DI/Impl/InitDependenciesInvoker.h"
#include "DI/Impl/InitMethodTypologyDeducer.h"

template<typename TObject>
void UnrealDI_Impl::TInstanceInjector<TObject>::Invoke(UObject& TargetObject, const FRegistrationStorage& Resolver)
{
    using Invoker = UnrealDI_Impl::TInitDependenciesInvoker<TObject, UnrealDI_Impl::TInitMethodTypologyDeducer< TObject >>;
    Invoker::Invoke((TObject*)&TargetObject, Resolver);
}
