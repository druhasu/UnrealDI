// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "Templates/Function.h"

class UClass;
class UObject;

namespace UnrealDI_Impl
{
    class FRegistrationStorage;

    // Wrapper struct to hold pointer to blueprint class so GC won't destroy it
    struct FInstanceFactoryResult
    {
        TFunction<UObject* (FRegistrationStorage&)> Function;
        UClass* BlueprintClass;
    };
}