// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

namespace UnrealDI_Impl
{
    template <typename...>
    struct TArgumentPack
    {
        using Type = TArgumentPack;
    };
}