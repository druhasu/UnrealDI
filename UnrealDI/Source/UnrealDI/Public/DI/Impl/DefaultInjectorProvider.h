// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "DI/IInjectorProvider.h"
#include "DefaultInjectorProvider.generated.h"

/*
 * Default implementation of IInjectorProvider that returns IInjector from container that created this object
 */
UCLASS()
class UDefaultInjectorProvider : public UObject, public IInjectorProvider
{
    GENERATED_BODY()

public:
    void InitDependencies(TScriptInterface<IInjector>&& InInjector);

    TScriptInterface<IInjector> GetInjector(UObject* InjectTarget) const override;

private:
    UPROPERTY()
    TScriptInterface<IInjector> Injector;
};
