// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "DI/Impl/HasInitDependencies.h"
#include "DI/Impl/IsSupportedArgument.h"
#include "DI/Impl/ArgumentPack.h"

namespace UnrealDI_Impl
{

    namespace Details
    {

        template <typename T, typename... TArgs>
        struct TSupportChecker
        {
            static constexpr bool Supported = UnrealDI_Impl::TIsSupportedArgument<T>::Value && TSupportChecker<TArgs...>::Supported;
        };

        template <typename T>
        struct TSupportChecker<T>
        {
            static constexpr bool Supported = UnrealDI_Impl::TIsSupportedArgument<T>::Value;
        };

        template <>
        struct TSupportChecker< TArgumentPack<> >
        {
            static constexpr bool Supported = true;
        };

        template <typename... TArgs>
        struct TSupportChecker< TArgumentPack<TArgs...> >
        {
            static constexpr bool Supported = TSupportChecker<TArgs...>::Supported;
        };

        template <typename T>
        using TInitDependenciesArgs = decltype(GetFunctionArgumentPack(&T::InitDependencies));

        template <typename T, typename = void>
        struct TInitMethodTypologyDeducer;

        struct FInitDependenciesSignatureNotSupported
        {
            using Type = FInitDependenciesSignatureNotSupported;
        };

        template <typename T>
        struct TInitMethodTypologyDeducer
        <
            T,
            typename TEnableIf<  THasNoInitDependencies< T >::Value >::Type
        > : TArgumentPack<>
        {};

        template <typename T>
        struct TInitMethodTypologyDeducer
        <
            T,
            typename TEnableIf< !THasNoInitDependencies< T >::Value &&  TSupportChecker< TInitDependenciesArgs<T> >::Supported >::Type
        > : TInitDependenciesArgs< T >
        {};

        template <typename T>
        struct TInitMethodTypologyDeducer
        <
            T,
            typename TEnableIf< !THasNoInitDependencies< T >::Value && !TSupportChecker< TInitDependenciesArgs<T> >::Supported >::Type
        > : FInitDependenciesSignatureNotSupported
        {};

    }

    /*
     * Detects how many arguments T::InitDependencies() have and whether it is supported
     * In case of success this type will extend TArgumentPack with corresponding amount of template arguments
     */
    template <typename T>
    using TInitMethodTypologyDeducer = typename Details::TInitMethodTypologyDeducer< T >::Type;

}