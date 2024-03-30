// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "UObject/Interface.h"
#include "UObject/ScriptInterface.h"
#include "IInjectorProvider.generated.h"

class IInjector;

UINTERFACE(MinimalApi)
class UInjectorProvider : public UInterface { GENERATED_BODY() };

class IInjectorProvider
{
    GENERATED_BODY()

public:
    /* Returns Injector that should be used for given object */
    virtual TScriptInterface<IInjector> GetInjector(UObject* InjectTarget) const = 0;
};
