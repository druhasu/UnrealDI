// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "UObject/SoftObjectPtr.h"

class UClass;
class UObject;

namespace UnrealDI_Impl
{
    class FRegistrationStorage;

    // Wrapper struct to hold pointers to a factory function and to blueprint class so we can reload it if GC destroyed it
    struct FInstanceFactoryInvoker
    {
        using FunctionPtr = UObject* (*)(FRegistrationStorage&, UClass*);

        UObject* Invoke(FRegistrationStorage& Resolver)
        {
            return Function(Resolver, (UClass*)EffectiveClassPtr.LoadSynchronous());
        }

        FunctionPtr Function;
        FSoftObjectPtr EffectiveClassPtr;
    };
}