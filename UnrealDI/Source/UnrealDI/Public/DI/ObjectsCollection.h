// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "HAL/UnrealMemory.h"
#include "DI/Impl/IsUInterface.h"
#include "DI/Impl/StaticClass.h"

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
        return T{ *Data };
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
 * Contains a collection of objects that were resolved
 */
template<typename T>
class TObjectsCollection
{
    template<typename U> static typename TEnableIf< TIsDerivedFrom< U, UObject >::Value, U* >::Type GetReturnType();
    template<typename U> static typename TEnableIf< UnrealDI_Impl::TIsUInterface< U >::Value, TScriptInterface< U > >::Type GetReturnType();

public:
    using ReturnType = decltype(GetReturnType<T>());

    TObjectsCollection()
    {
    }

    TObjectsCollection(UObject** Data, int32 Count)
        : Data(Data)
        , Count(Count)
    {
    }

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

    bool IsValid()
    {
        return Data != nullptr;
    }

    int32 Num()
    {
        return Count;
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

    TObjectsCollectionIterator<ReturnType> begin()       { return TObjectsCollectionIterator<ReturnType>(Data); }
    TObjectsCollectionIterator<ReturnType> begin() const { return TObjectsCollectionIterator<ReturnType>(Data); }

    TObjectsCollectionIterator<ReturnType> end()         { return TObjectsCollectionIterator<ReturnType>(Data + Count); }
    TObjectsCollectionIterator<ReturnType> end() const   { return TObjectsCollectionIterator<ReturnType>(Data + Count); }

private:
    template<typename U> friend class TObjectsCollection;

    // we are storing only pointers to UObjects, conversions to TScriptInterface are done during iteration
    UObject** Data;
    int32 Count;
};
