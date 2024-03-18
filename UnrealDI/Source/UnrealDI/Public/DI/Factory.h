// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "DI/Impl/IsUInterface.h"
#include "DI/Impl/StaticClass.h"
#include "UObject/ScriptInterface.h"

namespace UnrealDI_Impl
{
    template <typename T> struct TDependencyResolver;
    class FRegistrationStorage;

    /*
     * This struct allows us to use forward declared types in TFactory parameters.
     * Full definition of that type is only required when actually calling operator()
     */
    struct FFactoryCallProxy
    {
        /* For UObject */
        template <typename T>
        static typename TEnableIf< TIsDerivedFrom< T, UObject >::Value, T* >::Type
            Get(const FRegistrationStorage* Resolver);

        /* For TScriptInterface */
        template <typename T>
        static typename TEnableIf< TIsUInterface< T >::Value, TScriptInterface< T > >::Type
            Get(const FRegistrationStorage* Resolver);
    };
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
public:
    TFactory() = default;

    auto operator()() const;

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
    
    TFactory(const UnrealDI_Impl::FRegistrationStorage& Resolver);

    TWeakObjectPtr<UObject> ContainerPtr;
    const UnrealDI_Impl::FRegistrationStorage* Resolver = nullptr;
};

#include "DI/Impl/RegistrationStorage.h"
#include "DI/Impl/StaticClass.h"

template <typename T>
TFactory<T>::TFactory(const UnrealDI_Impl::FRegistrationStorage& Resolver)
    : ContainerPtr(Resolver.GetOwner())
    , Resolver(&Resolver)
{
}

template <typename T>
auto TFactory<T>::operator()() const
{
    UE_STATIC_ASSERT_COMPLETE_TYPE(T, "Type T in TFactory<T> must be fully defined when calling operator(), not just forward declared. Are you missing an #include?");
    checkf(ContainerPtr.IsValid(), TEXT("Object factory invoked after UObjectContainer was destroyed"));
    return UnrealDI_Impl::FFactoryCallProxy::Get<T>(Resolver);
}

/* For UObject */
template <typename T>
typename TEnableIf< TIsDerivedFrom< T, UObject >::Value, T* >::Type
UnrealDI_Impl::FFactoryCallProxy::Get(const UnrealDI_Impl::FRegistrationStorage* Resolver)
{
    return (T*)(Resolver->Resolve(UnrealDI_Impl::TStaticClass<T>::StaticClass()));
}

template <typename T>
typename TEnableIf< UnrealDI_Impl::TIsUInterface< T >::Value, TScriptInterface< T > >::Type
UnrealDI_Impl::FFactoryCallProxy::Get(const UnrealDI_Impl::FRegistrationStorage* Resolver)
{
    return TScriptInterface< T >(Resolver->Resolve(UnrealDI_Impl::TStaticClass<T>::StaticClass()));
}
