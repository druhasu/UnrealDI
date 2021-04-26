// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "Templates/IsInvocable.h"

namespace UnrealDI_Impl
{
    /*
     * Check whether given type T has method InitDependencies accepting given arguments TArgs...
     */
    template <typename T, typename... TArgs>
    struct THasInitDependencies
    {
        template<typename U> static decltype(&U::InitDependencies) Test(U*);
        template<typename U> static char Test(...);

        static const bool Value = TIsInvocable< decltype(Test<T>(nullptr)), T&, TArgs... >::Value;
    };

    /*
     * Check whether given type T has no method InitDependencies
     */
    template <typename T>
    struct THasNoInitDependencies
    {
        template<typename U> static decltype(&U::InitDependencies) Test(U*);
        template<typename U> static char Test(...);

        static const bool Value = TIsSame< char, decltype(Test<T>(nullptr)) >::Value;
    };
}