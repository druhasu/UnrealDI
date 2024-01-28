// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

namespace UnrealDI_Impl
{
    template <typename...>
    struct TArgumentPack
    {
        using Type = TArgumentPack;
    };

    template <typename... TArgs>
    TArgumentPack<TArgs...> GetFunctionArgumentPack(void (*)(TArgs...))
    {
        return TArgumentPack<TArgs...>();
    }

    template <typename T, typename... TArgs>
    TArgumentPack<TArgs...> GetFunctionArgumentPack(void (T::*)(TArgs...))
    {
        return TArgumentPack<TArgs...>();
    }
}