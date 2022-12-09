// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "HAL/UnrealMemory.h"
#include "DI/Impl/IsUInterface.h"
#include "DI/Impl/StaticClass.h"

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
    template<typename U> static typename TEnableIf< TIsDerivedFrom< U, UObject >::Value, U* >::Type GetReturnType();
    template<typename U> static typename TEnableIf< UnrealDI_Impl::TIsUInterface< U >::Value, TScriptInterface< U > >::Type GetReturnType();

public:
    using ReturnType = decltype(GetReturnType<T>());

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
    TArray<ReturnType> ToArray() const
    {
        TArray<ReturnType> Result;
        ToArray(Result);
        return Result;
    }

    /*
     * Fills provided TArray with pointers to objects from this collection.
     */
    void ToArray(TArray<ReturnType>& OutArray) const
    {
        OutArray.Reserve(OutArray.Num() + Count);

        for (int32 i = 0; i < Count; ++i)
        {
            OutArray.Emplace(ReturnType{ *(Data + i) });
        }
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

//private:
    template<typename U> friend class TObjectsCollection;

    // we are storing only pointers to UObjects, conversions to TScriptInterface are done during iteration
    UObject** Data;
    int32 Count;
};
