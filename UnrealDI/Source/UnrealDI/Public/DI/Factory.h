// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "DI/Impl/IsUInterface.h"
#include "DI/Impl/StaticClass.h"

class UObject;
class UObjectContainer;

namespace UnrealDI_Impl
{
    template <typename T> struct TDependencyResolver;
}

/*
 * Template class to request a factory of a required type.
 * It is used instead of TFunction<T*()> and TFunction<TScriptInterface<T>()>.
 * Depending on a T it will return either T* or TScriptInterface<T>
 */
template <typename T>
class TFactory
{
    template<typename U> static typename TEnableIf< TIsDerivedFrom< U, UObject >::Value, U* >::Type GetReturnType();
    template<typename U> static typename TEnableIf< UnrealDI_Impl::TIsUInterface< U >::Value, TScriptInterface< U > >::Type GetReturnType();

    using ReturnType = decltype(GetReturnType<T>());

public:
    TFactory() = default;

    ReturnType operator()() const
    {
        checkf(ContainerPtr.IsValid(), TEXT("Object factory invoked after UObjectContainer was destroyed"));
        return ReturnType(ContainerPtr->ResolveImpl<T>());
    }

    bool IsValid()
    {
        return ContainerPtr.IsValid();
    }

    operator bool()
    {
        return IsValid();
    }

private:
    template <typename U> friend struct UnrealDI_Impl::TDependencyResolver;
    
    TFactory(UObjectContainer& Resolver)
        : ContainerPtr(&Resolver)
    {
    }

    TWeakObjectPtr<UObjectContainer> ContainerPtr;
};