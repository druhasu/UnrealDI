// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "UnrealDIBlueprintLibrary.generated.h"

UCLASS()
class UNREALDI_API UUnrealDIBlueprintLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    /* Calls Function on an Object with provided variadic arguments */
    UFUNCTION(BlueprintCallable, CustomThunk, meta = (Variadic, BlueprintInternalUseOnly = "true"))
    static void CallFunctionIndirect(UObject* ThisObject, FName FunctionName) { checkNoEntry(); }

private:
    DECLARE_FUNCTION(execCallFunctionIndirect);
};
