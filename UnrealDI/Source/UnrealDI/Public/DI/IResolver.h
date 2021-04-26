// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "DI/Impl/StaticClass.h"
#include "DI/ObjectsCollection.h"
#include "UObject/Interface.h"
#include "Templates/EnableIf.h"
#include "Templates/Function.h"
#include "IResolver.generated.h"

UINTERFACE()
class UNREALDI_API UResolver : public UInterface { GENERATED_BODY() };

/*
 * Provides methods for resolving objects by Type and UClass
 */
class UNREALDI_API IResolver
{
    GENERATED_BODY()

public:
    /* Returns instance of given Type */
    virtual UObject* Resolve(class UClass* Type) = 0;

    /* Returns instance of given Type */
    template <typename T>
    typename TEnableIf<TIsDerivedFrom<T, UObject>::Value, T*>::Type
        Resolve()
    {
        return (T*)Resolve(UnrealDI_Impl::TStaticClass< T >::StaticClass());
    }

    /* Returns instance of given Interface */
    template <typename T>
    typename TEnableIf<UnrealDI_Impl::TIsUInterface< T >::Value, TScriptInterface< T >>::Type
        Resolve()
    {
        return Resolve(UnrealDI_Impl::TStaticClass< T >::StaticClass());
    }


    /* Returns all instances of given Type */
    virtual TObjectsCollection<UObject> ResolveAll(UClass* Type) = 0;

    /* Returns all instances of given Type */
    template <typename T>
    TObjectsCollection<T> ResolveAll()
    {
        return TObjectsCollection<T>(ResolveAll(UnrealDI_Impl::TStaticClass< T >::StaticClass()));
    }


    /* Returns true if given type is Registered and can be resolved */
    virtual bool IsRegistered(class UClass* Type) = 0;

    /* Returns true if given type is Registered and can be resolved */
    template <typename T>
    bool IsRegistered()
    {
        return IsRegistered(UnrealDI_Impl::TStaticClass<T>::StaticClass());
    }
};