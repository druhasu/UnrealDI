// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "DI/IResolver.h"
#include "DI/ObjectsCollection.h"
#include "DI/Factory.h"
#include "DI/Impl/StaticClass.h"
#include "DI/Impl/IsUInterface.h"
#include "UObject/ScriptInterface.h"
#include "UObject/ObjectPtr.h"

template <typename T, typename TCondition = void>
struct TDependencyResolver;

/* USomeClass* */
template <typename T>
struct TDependencyResolver
<
    T*,
    typename TEnableIf< TIsDerivedFrom< T, UObject >::Value >::Type
>
{
    static T* Resolve(const IResolver& Resolver)
    {
        return Cast<T>(Resolver.Resolve(UnrealDI_Impl::TStaticClass<T>::StaticClass()));
    }
};

/* TObjectPtr<USomeClass> */
template <typename T>
struct TDependencyResolver
<
    TObjectPtr<T>,
    typename TEnableIf< TIsDerivedFrom< T, UObject >::Value >::Type
>
{
    static TObjectPtr<T> Resolve(const IResolver& Resolver)
    {
        return Cast<T>(Resolver.Resolve(UnrealDI_Impl::TStaticClass<T>::StaticClass()));
    }
};

/* TScriptInterface<ISomeInterface> */
template <typename T>
struct TDependencyResolver
<
    TScriptInterface<T>,
    typename TEnableIf< UnrealDI_Impl::TIsUInterface< T >::Value >::Type
>
{
    static TScriptInterface<T> Resolve(const IResolver& Resolver)
    {
        return Resolver.Resolve(UnrealDI_Impl::TStaticClass<T>::StaticClass());
    }
};

/* TObjectsCollection<USomeClass> or TObjectsCollection<ISomeInterface> */
template <typename T>
struct TDependencyResolver
<
    TObjectsCollection<T>,
    typename TEnableIf< TOr< TIsDerivedFrom< T, UObject >, UnrealDI_Impl::TIsUInterface< T > >::Value >::Type
>
{
    static TObjectsCollection<T> Resolve(const IResolver& Resolver)
    {
        return Resolver.ResolveAll(UnrealDI_Impl::TStaticClass<T>::StaticClass());
    }
};

/* TFactory<USomeClass> or TFactory<ISomeInterface> */
template <typename T>
struct TDependencyResolver
<
    TFactory<T>,
    typename TEnableIf< TOr< TIsDerivedFrom< T, UObject >, UnrealDI_Impl::TIsUInterface< T > >::Value >::Type
>
{
    static TFactory<T> Resolve(const IResolver& Resolver)
    {
        return TFactory<T>(&Resolver);
    }
};
