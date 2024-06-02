// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "DI/Impl/IsUInterface.h"
#include "DI/Impl/StaticClass.h"
#include "DI/IResolver.h"
#include "UObject/ScriptInterface.h"
#include "UObject/WeakInterfacePtr.h"

/*
 * Template class to request a factory of a required type.
 * It is used instead of TFunction<T*()> and TFunction<TScriptInterface<T>()>.
 * Depending on a T it will return either T* or TScriptInterface<T>
 */
template <typename T>
class TFactory
{
public:
    TFactory(const IResolver* InResolver = nullptr)
        : WeakResolver(InResolver ? InResolver->_getUObject() : nullptr)
    {
    }

    /*
     * Resolves instance of type T. Asserts if container is no longer valid
     */
    auto operator()() const
    {
        UE_STATIC_ASSERT_COMPLETE_TYPE(T, "Type T in TFactory<T> must be fully defined when calling operator(), not just forward declared. Are you missing an #include?");

        IResolver* Resolver = WeakResolver.Get();
        checkf(Resolver != nullptr, TEXT("TFactory invoked after UObjectContainer was destroyed"));

        return Cast(Resolver->Resolve(UnrealDI_Impl::TStaticClass<T>::StaticClass()));
    }

    /*
     * Resolves instance of type T or nullptr if it is not registered. Asserts if container is no longer valid
     * Use this method ONLY for optional dependencies
     */
    auto TryResolve() const
    {
        UE_STATIC_ASSERT_COMPLETE_TYPE(T, "Type T in TFactory<T> must be fully defined when calling TryResolve(), not just forward declared. Are you missing an #include?");

        IResolver* Resolver = WeakResolver.Get();
        checkf(Resolver != nullptr, TEXT("TFactory invoked after UObjectContainer was destroyed"));

        if (Resolver->IsRegistered<T>())
        {
            return Cast(Resolver->Resolve(UnrealDI_Impl::TStaticClass<T>::StaticClass()));
        }

        return Cast(nullptr);
    }

    /*
     * Checks whether this Factory is Vallid.
     * This means Container that created it is alive, and type T is registered in it
     */
    bool IsValid() const
    {
        UE_STATIC_ASSERT_COMPLETE_TYPE(T, "Type T in TFactory<T> must be fully defined when calling IsValid(), not just forward declared. Are you missing an #include?");

        IResolver* Resolver = WeakResolver.Get();
        return Resolver != nullptr && Resolver->IsRegistered<T>();
    }

    /*
     * Checks whether this Factory is Vallid.
     * This means Container that created it is alive, and type T is registered in it
     */
    operator bool() const
    {
        return IsValid();
    }

private:
    auto Cast(UObject* Object) const
    {
        if constexpr (TIsDerivedFrom< T, UObject >::Value)
        {
            return (T*)Object;
        }
        else if constexpr (UnrealDI_Impl::TIsUInterface< T >::Value)
        {
            return TScriptInterface< T >(Object);
        }
        else
        {
            return Object;
        }
    }

    TWeakInterfacePtr<IResolver> WeakResolver;
};
