// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "MockClasses.h"
#include "MockClasses_AutoRegister.generated.h"

UCLASS()
class UWithAutoRegisteredDependency : public UObject
{
    GENERATED_BODY()

public:
    void InitDependencies(UNeedInterfaceInstance* InInstance)
    {
        Instance = InInstance;
    }

    UNeedInterfaceInstance* Instance;
};
