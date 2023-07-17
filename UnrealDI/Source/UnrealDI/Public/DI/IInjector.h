// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "UObject/Interface.h"
#include "IInjector.generated.h"

UINTERFACE()
class UNREALDI_API UInjector : public UInterface { GENERATED_BODY() };

/*
 * Provides methods for injecting dependencies into already created Objects.
 * Those Objects' classes must be registered in container using RegisterInjector method
 */
class UNREALDI_API IInjector
{
    GENERATED_BODY()

public:

    /*
     * Injects dependencies into an Object.
     * Returns true if Object registration was found in container, false otherwise
     */
    virtual bool Inject(UObject* Object) = 0;
};