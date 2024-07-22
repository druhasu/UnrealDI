// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "UnrealDIBlueprintLibrary.generated.h"

UCLASS(meta = (NoInitDependencies))
class UNREALDI_API UUnrealDIBlueprintLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    /* Tries to call InitDependencies on provided object, resolving them from current World-bound container */
    UFUNCTION(BlueprintCallable, Category = "Dependency Injection", meta = (HidePin = "Target", DefaultToSelf = "Target", CompactNodeTitle = "Try Init Dependencies"))
    static void TryInitDependencies(UObject* Target);

    /* Calls Function on an Object with provided variadic arguments */
    UFUNCTION(BlueprintCallable, CustomThunk, meta = (Variadic, BlueprintInternalUseOnly = "true"))
    static void CallFunctionIndirect(UObject* ThisObject, FName FunctionName) { checkNoEntry(); }

private:
    DECLARE_FUNCTION(execCallFunctionIndirect);
};
