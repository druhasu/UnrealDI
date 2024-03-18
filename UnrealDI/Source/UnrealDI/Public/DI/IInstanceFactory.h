// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "UObject/Interface.h"
#include "IInstanceFactory.generated.h"

UINTERFACE(MinimalApi)
class UInstanceFactory : public UInterface { GENERATED_BODY() };

/*
 * Allows creation of Objects of given class.
 * Implement in your game to provide custom creation logic for game specific classes
 * Register in container using this example:
 *
 *   Builder.RegisterType<UInstanceFactory_MyGameSpecificClass>().As<IInstanceFactory>();
 *
 * If you register multiple classes, last ones have higher priority.
 * Custom registered classes have priority over builtin ones.
 */
class UNREALDI_API IInstanceFactory
{
    GENERATED_BODY()

public:
    /* Whether given class is supported by this factory. First factory that returns true will be picked for creation */
    virtual bool IsClassSupported(UClass* EffectiveClass) const = 0;

    /* Creates new Object of specified class and does minimal required initialization */
    virtual UObject* Create(UObject* Outer, UClass* EffectiveClass) const = 0;

    /* Performs final initialization of created object. This method is called after InitDependencies are called on Object */
    virtual void FinalizeCreation(UObject* Object) const = 0;
};
