// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "UObject/UObjectGlobals.h"
#include "DI/Impl/HasInitDependencies.h"

namespace UnrealDI_Impl
{
    template<bool, typename T, typename... TArgs>
    struct TNewObjectHelper;

    template<typename T, typename... TArgs>
    struct TNewObjectHelper<false, T, TArgs...>
    {
        static T* NewObjectWithArgs(UObject* Outer, FName Name, TArgs... Args)
        {
            static_assert(false, "Type need to implement method InitDependencies accepting required arguments. See below for more information");
        }
    };

    template<typename T, typename... TArgs>
    struct TNewObjectHelper<true, T, TArgs...>
    {
        FORCEINLINE static T* NewObjectWithArgs(UObject* Outer, FName Name, TArgs... Args)
        {
            auto ret = NewObject<T>(Outer, Name);
            ret->InitArgs(Args...);
            return ret;
        }
    };
}

/*
 * Analogue of NewObject that accepts arguments
 */
template<typename T, typename... TArgs>
FORCEINLINE T* NewObjectWithArgs(FName Name, UObject* Outer, TArgs... Args)
{
    using Helper = UnrealDI_Impl::TNewObjectHelper< UnrealDI_Impl::THasInitDependencies< T, TArgs... >::Value, T, TArgs... >;
    return Helper::NewObjectWithArgs(Outer, Name, Args...);
}

/*
 * Analogue of NewObject that accepts arguments
 */
template<typename T, typename... TArgs>
FORCEINLINE T* NewObjectWithArgs(UObject* Outer, TArgs... Args)
{
    using Helper = UnrealDI_Impl::TNewObjectHelper< UnrealDI_Impl::THasInitDependencies< T, TArgs... >::Value, T, TArgs... >;
    return Helper::NewObjectWithArgs(Outer, NAME_None, Args...);
}
