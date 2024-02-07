// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "HAL/UnrealMemory.h"
#include "DI/Impl/IsUInterface.h"
#include "DI/Impl/StaticClass.h"
#include "UObject/ScriptInterface.h"

template<typename T>
class TObjectsCollectionIterator;

namespace UnrealDI_Impl
{
    /*
     * This struct allows us to use forward declared types in TObjectsCollection parameters.
     * Full definition of that type is only required when actually calling ToArray()
     */
    struct FObjectsCollectionCallProxy
    {
        /* For UObject */
        template <typename T>
        static typename TEnableIf< TIsDerivedFrom< T, UObject >::Value, TArray<T*> >::Type
            ToArray(UObject** Data, int32 Count);

        /* For UObject */
        template <typename T>
        static typename TEnableIf< TIsDerivedFrom< T, UObject >::Value, void >::Type
            ToArray(UObject** Data, int32 Count, TArray<T*>& OutArray);

        /* For UObject */
        template <typename T>
        static typename TEnableIf< TIsDerivedFrom< T, UObject >::Value, TObjectsCollectionIterator<T*> >::Type
            CreateIterator(UObject** Data);

        /* For TScriptInterface */
        template <typename T>
        static typename TEnableIf< TIsUInterface< T >::Value, TArray< TScriptInterface<T> > >::Type
            ToArray(UObject** Data, int32 Count);

        /* For TScriptInterface */
        template <typename T>
        static typename TEnableIf< TIsUInterface< T >::Value, void >::Type
            ToArray(UObject** Data, int32 Count, TArray< TScriptInterface<T> >& OutArray);

        /* For TScriptInterface */
        template <typename T>
        static typename TEnableIf< TIsUInterface< T >::Value, TObjectsCollectionIterator< TScriptInterface<T> > >::Type
            CreateIterator(UObject** Data);

        template <typename T>
        static TArray<T> ToArrayImpl(UObject** Data, int32 Count);

        template <typename T>
        static void ToArrayImpl(UObject** Data, int32 Count, TArray<T>& OutArray);
    };
}

/*
 * Iterator for TObjectsCollection
 */
template<typename T>
class TObjectsCollectionIterator
{
public:
    TObjectsCollectionIterator(UObject** Data)
        : Data(Data)
    {
    }

    T operator*() const
    {
        return (T) (*Data);
    }

    TObjectsCollectionIterator& operator++()
    {
        ++Data;
        return *this;
    }

private:
    friend bool operator!=(const TObjectsCollectionIterator& Lhs, const TObjectsCollectionIterator& Rhs)
    {
        return Lhs.Data != Rhs.Data;
    }

    UObject** Data;
};

/*
 * Contains a collection of objects that were resolved.
 * This collection may be iterated with range based for loop.
 */
template<typename T>
class TObjectsCollection
{
public:
    /*
     * Constructs empty collection
     */
    TObjectsCollection()
        : Data(nullptr)
    {
    }

    /*
     * Constructs collection from memory and count.
     * Collection will own the memory and will free it when destroyed.
     */
    TObjectsCollection(UObject** Data, int32 Count)
        : Data(Data)
        , Count(Count)
    {
    }

    /*
     * Move constructs from other collection
     */
    template<typename U>
    TObjectsCollection(TObjectsCollection<U>&& Other)
        : Data(Other.Data)
        , Count(Other.Count)
    {
        Other.Data = nullptr;
        Other.Count = 0;
    }

    ~TObjectsCollection()
    {
        if (Data)
        {
            FMemory::Free(Data);
            Data = nullptr;
        }
    }

    /*
     * Returns true if collection has valid memory pointer.
     */
    bool IsValid() const
    {
        return Data != nullptr;
    }

    /*
     * Returns amount of objects in a collection.
     */
    int32 Num() const
    {
        return Count;
    }

    /*
     * Converts this collection to TArray.
     */
    auto ToArray() const
    {
        UE_STATIC_ASSERT_COMPLETE_TYPE(T, "Type T in TObjectsCollection<T> must be fully defined when calling ToArray(), not just forward declared. Are you missing an #include?");
        return UnrealDI_Impl::FObjectsCollectionCallProxy::ToArray<T>(Data, Count);
    }

    /*
     * Fills provided TArray with pointers to objects from this collection.
     */
    template <typename U>
    void ToArray(TArray<U>& OutArray) const
    {
        UE_STATIC_ASSERT_COMPLETE_TYPE(T, "Type T in TObjectsCollection<T> must be fully defined when calling ToArray(), not just forward declared. Are you missing an #include?");
        UnrealDI_Impl::FObjectsCollectionCallProxy::ToArray<T>(Data, Count, OutArray);
    }

    // Non-copyable
    TObjectsCollection(const TObjectsCollection&) = delete;
    TObjectsCollection& operator=(const TObjectsCollection&) = delete;

    template<typename U>
    TObjectsCollection& operator=(TObjectsCollection<U>&& Other)
    {
        Data = Other.Data;
        Count = Other.Count;
        Other.Data = nullptr;
        Other.Count = 0;
        return *this;
    }

    auto begin()       { return UnrealDI_Impl::FObjectsCollectionCallProxy::CreateIterator<T>(Data); }
    auto begin() const { return UnrealDI_Impl::FObjectsCollectionCallProxy::CreateIterator<T>(Data); }

    auto end()         { return UnrealDI_Impl::FObjectsCollectionCallProxy::CreateIterator<T>(Data + Count); }
    auto end() const   { return UnrealDI_Impl::FObjectsCollectionCallProxy::CreateIterator<T>(Data + Count); }

private:
    template<typename U> friend class TObjectsCollection;

    // we are storing only pointers to UObjects, conversions to TScriptInterface are done during iteration
    UObject** Data;
    int32 Count;
};


/* For UObject */
template <typename T>
typename TEnableIf< TIsDerivedFrom< T, UObject >::Value, TArray< T* > >::Type
UnrealDI_Impl::FObjectsCollectionCallProxy::ToArray(UObject** Data, int32 Count)
{
    return ToArrayImpl<T*>(Data, Count);
}

/* For UObject */
template <typename T>
typename TEnableIf< TIsDerivedFrom< T, UObject >::Value, void >::Type 
UnrealDI_Impl::FObjectsCollectionCallProxy::ToArray(UObject** Data, int32 Count, TArray<T*>& OutArray)
{
    ToArrayImpl<T*>(Data, Count, OutArray);
}

/* For UObject */
template <typename T>
typename TEnableIf< TIsDerivedFrom< T, UObject >::Value, TObjectsCollectionIterator<T*> >::Type
UnrealDI_Impl::FObjectsCollectionCallProxy::CreateIterator(UObject** Data)
{
    return TObjectsCollectionIterator< T* >(Data);
}

/* For TScriptInterface */
template <typename T>
typename TEnableIf< UnrealDI_Impl::TIsUInterface< T >::Value, TArray< TScriptInterface<T> > >::Type
UnrealDI_Impl::FObjectsCollectionCallProxy::ToArray(UObject** Data, int32 Count)
{
    return ToArrayImpl<TScriptInterface<T>>(Data, Count);
}

/* For TScriptInterface */
template <typename T>
typename TEnableIf< UnrealDI_Impl::TIsUInterface< T >::Value, void >::Type
UnrealDI_Impl::FObjectsCollectionCallProxy::ToArray(UObject** Data, int32 Count, TArray< TScriptInterface<T> >& OutArray)
{
    ToArrayImpl<TScriptInterface<T>>(Data, Count, OutArray);
}

/* For TScriptInterface */
template <typename T>
typename TEnableIf< UnrealDI_Impl::TIsUInterface< T >::Value, TObjectsCollectionIterator< TScriptInterface<T> > >::Type
UnrealDI_Impl::FObjectsCollectionCallProxy::CreateIterator(UObject** Data)
{
    return TObjectsCollectionIterator< TScriptInterface<T> >(Data);
}

template <typename T>
TArray<T> UnrealDI_Impl::FObjectsCollectionCallProxy::ToArrayImpl(UObject** Data, int32 Count)
{
    TArray<T> Result;
    ToArrayImpl<T>(Data, Count, Result);
    return Result;
}

template <typename T>
void UnrealDI_Impl::FObjectsCollectionCallProxy::ToArrayImpl(UObject** Data, int32 Count, TArray<T>& OutArray)
{
    OutArray.Reserve(OutArray.Num() + Count);

    for (int32 i = 0; i < Count; ++i)
    {
        OutArray.Emplace((T)(*(Data + i)));
    }
}
