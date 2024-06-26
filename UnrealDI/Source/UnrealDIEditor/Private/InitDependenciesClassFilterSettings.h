// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "InitDependenciesClassFilterSettings.generated.h"

/*
 * Contains settings for InitDependencies class filter so they persist across sessions
 */
UCLASS(Config = "UnrealDI")
class UInitDependenciesClassFilterSettings : public UObject
{
    GENERATED_BODY()

public:
    void SetShowOnlyProjectClasses(bool bValue)
    {
        bShowOnlyProjectClasses = bValue;
        SaveConfig();
    }

    void SetShowOnlyInterfaces(bool bValue)
    {
        bShowOnlyInterfaces = bValue;
        SaveConfig();
    }

    UPROPERTY(Config)
    bool bShowOnlyProjectClasses = true;

    UPROPERTY(Config)
    bool bShowOnlyInterfaces = true;
};
