// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "DI/Impl/IsUInterface.h"
#include "UObject/ObjectPtr.h"
#include "Templates/UnrealTypeTraits.h"

template <typename T> class TFactory;
template <typename T> class TObjectsCollection;

namespace UnrealDI_Impl
{
    namespace Details
    {

        // treat all arguments as unsupported by default
        template <typename T>
        struct TIsSupportedArgument : TIntegralConstant< bool, false > {};

        // support T* - single instance of UObject
        template <typename T>
        struct TIsSupportedArgument< T* > : TIsDerivedFrom< T, UObject > {};

        // support TObjectPtr<T> - single instance of UObject
        template <typename T>
        struct TIsSupportedArgument< TObjectPtr<T> > : TIsDerivedFrom< T, UObject > {};

        // support TScriptInterface< T > - single instance of UInterface
        template <typename T>
        struct TIsSupportedArgument< TScriptInterface< T > > : TIsUInterface< T > {};

        // support TObjectsCollection< T > - all instances of either UObject or UInterface
        template <typename T>
        struct TIsSupportedArgument< TObjectsCollection< T > > : TOr< TIsDerivedFrom< T, UObject >, TIsUInterface< T > > {};

        // support TFactory< T > - instance factory of either UObject or UInterface
        template <typename T>
        struct TIsSupportedArgument< TFactory< T > > : TOr< TIsDerivedFrom< T, UObject >, TIsUInterface< T > > {};

    }

    /*
     * Checks whether T is supported type for InitDependencies argument
     * Allows using T&, const T& or T&& if T itself is supported
     */
    template <typename T>
    using TIsSupportedArgument = typename Details::TIsSupportedArgument< typename TDecay< T >::Type >;

}