// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "Templates/UnrealTypeTraits.h"
#include <type_traits>

namespace UnrealDI_Impl
{
    /*
     * Checks whether given type is IInterface of UInterface type
     * so: TIsUInterface<IMyInterface>::Value == true, if there is corresponding UMyInterface
     */
    template <typename T>
    struct TIsUInterface
    {
        template<typename U> static decltype(&U::_getUObject) Test(U*);
        template<typename U> static char Test(...);

        // Here we check whether given type T has _getUObject() method and is not a UObject itself.
        // We need to check for UObject, because interface implementations inherit _getUObject() from interface base class
        static const bool Value = !std::is_same< char, decltype(Test<T>(nullptr)) >::value && !TIsDerivedFrom<T, UObject>::Value;
    };

}