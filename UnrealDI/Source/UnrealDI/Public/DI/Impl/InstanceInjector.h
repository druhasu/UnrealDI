// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

class UObject;
class IResolver;

namespace UnrealDI_Impl
{
    /* Generator class for injector functions */
    template<typename TObject>
    struct TInstanceInjector
    {
        static void Invoke(UObject& TargetObject, const IResolver& Resolver);
    };
}

#include "DI/Impl/InitDependenciesInvoker.h"
#include "DI/Impl/InitMethodTypologyDeducer.h"

template<typename TObject>
void UnrealDI_Impl::TInstanceInjector<TObject>::Invoke(UObject& TargetObject, const IResolver& Resolver)
{
    using Invoker = UnrealDI_Impl::TInitDependenciesInvoker<TObject, UnrealDI_Impl::TInitMethodTypologyDeducer< TObject >>;
    Invoker::Invoke((TObject*)&TargetObject, Resolver);
}
