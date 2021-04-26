// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "DI/Impl/IsUInterface.h"
#include "Templates/EnableIf.h"

namespace UnrealDI_Impl
{
    /*
     * Helper Struct to access ::StaticClass() of UObject and IInterface in a generic manner
     */ 
    template <typename T, typename = void>
    struct TStaticClass;

    template <typename T>
    struct TStaticClass<T, typename TEnableIf< TIsUInterface< T >::Value >::Type >
    {
        using Type = typename T::UClassType;

        static UClass* StaticClass() { return Type::StaticClass(); }
    };

    template <typename T>
    struct TStaticClass<T, typename TEnableIf< TIsDerivedFrom< T, UObject >::Value >::Type >
    {
        using Type = typename T;

        static UClass* StaticClass() { return Type::StaticClass(); }
    };

}