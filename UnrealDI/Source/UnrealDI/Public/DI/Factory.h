// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "DI/Impl/IsUInterface.h"
#include "DI/Impl/StaticClass.h"
#include "UObject/ScriptInterface.h"

/*
 * Template class to request a factory of a required type.
 * It is used instead of TFunction<T*()> and TFunction<TScriptInterface<T>()>.
 * Depending on a T it will return either T* or TScriptInterface<T>
 */
template <typename T>
class TFactory
{
public:
    using FFactoryFunctionPtr = UObject* (*)(const UObject& Context, UClass& ObjectClass);

    TFactory() = default;

    TFactory(const UObject& Object, FFactoryFunctionPtr FactoryFunction)
        : WeakContextObject(&Object)
        , FactoryFunction(FactoryFunction)
    {}

    template <typename U>
    explicit TFactory(const TFactory<U>& Other)
        : WeakContextObject(Other.WeakContextObject)
        , FactoryFunction(Other.FactoryFunction)
    {}

    template <typename U>
    TFactory(TFactory<U>&& Other)
        : WeakContextObject(Other.WeakContextObject)
        , FactoryFunction(Other.FactoryFunction)
    {}

    /*
     * Resolves instance of type T. Asserts if container is no longer valid
     */
    auto operator()() const
    {
        UE_STATIC_ASSERT_COMPLETE_TYPE(T, "Type T in TFactory<T> must be fully defined when calling operator(), not just forward declared. Are you missing an #include?");

        checkf(FactoryFunction != nullptr, TEXT("TFactory is not initialized"));

        const UObject* ContextObject = WeakContextObject.Get();
        checkf(ContextObject != nullptr, TEXT("TFactory invoked after UObjectContainer was destroyed"));

        return Cast(FactoryFunction(*ContextObject, *UnrealDI_Impl::TStaticClass<T>::StaticClass()));
    }

    /*
     * Checks whether this Factory is Vallid.
     * This means Container that created it is alive, and type T is registered in it
     */
    bool IsValid() const
    {
        return FactoryFunction != nullptr && WeakContextObject.IsValid();
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
    template<typename U> friend class TFactory;

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

    TWeakObjectPtr<const UObject> WeakContextObject;
    FFactoryFunctionPtr FactoryFunction = nullptr;
};
