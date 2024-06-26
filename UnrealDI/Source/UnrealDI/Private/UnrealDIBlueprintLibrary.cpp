// Copyright Andrei Sudarikov. All Rights Reserved.

#include "DI/Impl/UnrealDIBlueprintLibrary.h"

DEFINE_FUNCTION(UUnrealDIBlueprintLibrary::execCallFunctionIndirect)
{
    /*
    * It is impossible to create new functions in a K2Node that are usable with K2Node_CallFunction
    * K2Node_CallFunction only uses functions from SkeletonClass, which cannot be modified by custom K2Nodes
    * So we use this function to call another function, specified by FunctionName argument, with parameters that are placed in stack by calling code
    * It is possible because this function is Variadic and may accept any amount of additional parameters
    */

    // Read "fixed" function arguments
    P_GET_OBJECT(UObject, ThisObject);
    P_GET_PROPERTY(FNameProperty, FunctionName);

    // Variadic arguments are still in Stack. We can simply pass them to the next call, because the are already in right format
    UFunction* InitDependenciesFunction = ThisObject->GetClass()->FindFunctionByName(FunctionName);
    ThisObject->CallFunction(Stack, RESULT_PARAM, InitDependenciesFunction);

    P_FINISH;
}
