// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "DI/Impl/IsUInterface.h"
#include "DI/Impl/StaticClass.h"

namespace UnrealDI_Impl
{
    template <typename T> struct TDependencyResolver;
    class FRegistrationStorage;
}

class UObjectContainer;

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

    ReturnType operator()() const;

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
    
    TFactory(UnrealDI_Impl::FRegistrationStorage& Resolver);

    TWeakObjectPtr<UObject> ContainerPtr;
    UnrealDI_Impl::FRegistrationStorage* Resolver = nullptr;
};

#include "DI/Impl/RegistrationStorage.h"

template <typename T>
TFactory<T>::TFactory(UnrealDI_Impl::FRegistrationStorage& Resolver)
    : ContainerPtr(Resolver.GetOwner())
    , Resolver(&Resolver)
{
}

template <typename T>
typename TFactory<T>::ReturnType TFactory<T>::operator()() const
{
    checkf(ContainerPtr.IsValid(), TEXT("Object factory invoked after UObjectContainer was destroyed"));
    return ReturnType(Resolver->ResolveImpl<T>());
}