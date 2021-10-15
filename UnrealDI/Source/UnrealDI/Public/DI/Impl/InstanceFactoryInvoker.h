// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

class UClass;
class UObject;

namespace UnrealDI_Impl
{
    class FRegistrationStorage;

    // Wrapper struct to hold pointers to a factory function and to blueprint class so GC won't destroy it
    struct FInstanceFactoryInvoker
    {
        using FunctionPtr = UObject* (*)(FRegistrationStorage&, UClass*);

        UObject* Invoke(FRegistrationStorage& Resolver)
        {
            return Function(Resolver, EffectiveClass);
        }

        FunctionPtr Function;
        UClass* EffectiveClass;
    };
}