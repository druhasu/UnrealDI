// Copyright Andrei Sudarikov. All Rights Reserved.

#include "DI/Impl/DefaultInjectorProvider.h"

void UDefaultInjectorProvider::InitDependencies(TScriptInterface<IInjector>&& InInjector)
{
    Injector = MoveTemp(InInjector);
}

TScriptInterface<IInjector> UDefaultInjectorProvider::GetInjector(UObject* InjectTarget) const
{
    return Injector;
}
