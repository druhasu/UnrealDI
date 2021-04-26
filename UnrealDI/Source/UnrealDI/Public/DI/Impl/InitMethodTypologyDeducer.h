// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "DI/Impl/HasInitDependencies.h"
#include "DI/Impl/IsSupportedArgument.h"
#include "DI/Impl/ArgumentPack.h"

#define IOC_MAX_DEPENDENCIES 20

namespace UnrealDI_Impl
{

    struct FConstructorTypologyNotSupported
    {
        using Type = FConstructorTypologyNotSupported;
    };

    template <typename TParent>
    struct TAnyArgument
    {
        typedef TParent Type;

        template
        <
            typename T,
            typename = typename TEnableIf
            <
                !std::is_convertible< TParent, T >::value&&
                TIsSupportedArgument< T >::Value
            >::Type
        >
        operator T()
        {
            // Nothing to do here, we just need a conversion operator to evaluate THasInitMethod
        }
    };

    namespace Details
    {
        template <typename T, int>
        struct TWrapAndGet : TAnyArgument< T > {};

        template <typename, typename, typename = void>
        struct TInitMethodTypologyDeducer;


        // Initial recursion state
        template <typename T>
        struct TInitMethodTypologyDeducer
        <
            T,
            TIntegerSequence< int >,
            typename TEnableIf< TOr< THasInitDependencies< T >, THasNoInitDependencies< T > >::Value >::Type
        > : TArgumentPack<>
        {};

        template <typename T>
        struct TInitMethodTypologyDeducer
        <
            T,
            TIntegerSequence< int >,
            typename TEnableIf< TAnd< TNot< THasInitDependencies< T > >, TNot< THasNoInitDependencies< T > > >::Value >::Type
        > : TInitMethodTypologyDeducer< T, TMakeIntegerSequence< int, 1 > >::Type
        {};


        // Common recursion state
        template <typename T, int... NthArgument>
        struct TInitMethodTypologyDeducer
        <
            T,
            TIntegerSequence< int, NthArgument... >,
            typename TEnableIf
            <
                (sizeof...(NthArgument) > 0) &&
                (sizeof...(NthArgument) < IOC_MAX_DEPENDENCIES) &&
                THasInitDependencies< T, TWrapAndGet< T, NthArgument >... >::Value
            >::Type
        > : TArgumentPack< TWrapAndGet< T, NthArgument >... >
        {};

        template <typename T, int... NthArgument>
        struct TInitMethodTypologyDeducer
        <
            T,
            TIntegerSequence< int, NthArgument... >,
            typename TEnableIf
            <
                (sizeof...(NthArgument) > 0) &&
                (sizeof...(NthArgument) < IOC_MAX_DEPENDENCIES) &&
                !THasInitDependencies< T, TWrapAndGet< T, NthArgument >... >::Value
            >::Type
        > : TInitMethodTypologyDeducer< T, TMakeIntegerSequence< int, sizeof...(NthArgument) + 1 > >::Type
        {};


        // Last recursion state
        template <typename T, int... NthArgument>
        struct TInitMethodTypologyDeducer
        <
            T,
            TIntegerSequence< int, NthArgument... >,
            typename TEnableIf
            <
                (sizeof...(NthArgument) == IOC_MAX_DEPENDENCIES) &&
                THasInitDependencies< T, TWrapAndGet< T, NthArgument >... >::Value
            >::Type
        > : TArgumentPack< TWrapAndGet< T, NthArgument >... >
        {};

        template <typename T, int... NthArgument>
        struct TInitMethodTypologyDeducer
        <
            T,
            TIntegerSequence< int, NthArgument... >,
            typename TEnableIf
            <
                (sizeof...(NthArgument) == IOC_MAX_DEPENDENCIES) &&
                !THasInitDependencies< T, TWrapAndGet< T, NthArgument >... >::Value
            >::Type
        > : FConstructorTypologyNotSupported
        {};

    }

    /*
     * Detects how many arguments T::InitDependencies() have and whether it is supported
     * In case of success this type will extend TArgumentPack with corresponding amount of template arguments
     */
    template <typename T>
    using TInitMethodTypologyDeducer = typename Details::TInitMethodTypologyDeducer< T, TMakeIntegerSequence< int, 0 > >::Type;

}