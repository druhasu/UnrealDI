// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "MockClasses.h"
#include "MockClasses_Injector.generated.h"

UCLASS(Blueprintable)
class UTestInjection_Base : public UObject
{
    GENERATED_BODY()

public:
    void InitDependencies(TScriptInterface<IReader> InInstance)
    {
        Instance = InInstance;
    }

    TScriptInterface<IReader> Instance;
};

UCLASS()
class UTestInjection_Derived : public UTestInjection_Base
{
    GENERATED_BODY()
};

UCLASS()
class UNREALDITESTS_API UTestInjection_Exposed : public UObject
{
    GENERATED_BODY()

public:
    void InitDependencies(TScriptInterface<IReader> InInstance)
    {
        Instance = InInstance;
    }

    TScriptInterface<IReader> Instance;
};