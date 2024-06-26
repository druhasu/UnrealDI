// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "UObject/Interface.h"
#include "IBlueprintDependencyInterface.generated.h"

UINTERFACE(BlueprintType, meta = (CannotImplementInterfaceInBlueprint))
class UBlueprintDependencyInterface : public UInterface { GENERATED_BODY() };

/* Interface used in testing InitDependencies node in blueprints */
class IBlueprintDependencyInterface
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    virtual FString GetValue() const = 0;
};
