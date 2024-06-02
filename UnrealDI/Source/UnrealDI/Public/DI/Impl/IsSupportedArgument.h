// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include <type_traits>

namespace UnrealDI_Impl
{
    namespace Details
    {

        template <typename T>
        struct THasDependencyResolver
        {
            template<typename U> static decltype(&TDependencyResolver<U>::Resolve) Test(U*);
            template<typename U> static char Test(...);

            static const bool Value = !std::is_same< char, decltype(Test<T>(nullptr)) >::value;
        };

    }

    /*
     * Checks whether T is supported type for InitDependencies argument
     * Allows using T&, const T& or T&& if T itself is supported
     */
    template <typename T>
    using TIsSupportedArgument = typename Details::THasDependencyResolver< typename TDecay< T >::Type >;

}
