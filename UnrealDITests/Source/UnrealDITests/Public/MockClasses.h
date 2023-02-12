// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "TestClasses.h"
#include "DI/Factory.h"
#include "DI/ObjectsCollection.h"
#include "MockClasses.generated.h"

/* Implements IReader interface */
UCLASS(Blueprintable)
class UNREALDITESTS_API UMockReader : public UObject, public IReader
{
    GENERATED_BODY()

public:
    FString Read() override { return NextValue; }

    FString NextValue;
};

/* Requests instance of Concrete type */
UCLASS()
class UNREALDITESTS_API UNeedObjectInstance : public UObject
{
    GENERATED_BODY()
public:
    void InitDependencies(UMockReader* ReaderConcrete)
    {
        Instance = ReaderConcrete;
    }

    UMockReader* Instance;
};

/* Requests instance of Concrete type via TObjectPtr */
UCLASS()
class UNREALDITESTS_API UNeedObjectPtrInstance : public UObject
{
    GENERATED_BODY()
public:

    // we cannot wrap whole class inside #if, so wrap only internals
#if ENGINE_MAJOR_VERSION >= 5
    void InitDependencies(TObjectPtr<UMockReader> ReaderConcrete)
    {
        Instance = ReaderConcrete;
    }

    TObjectPtr<UMockReader> Instance;
#endif
};

/* Requests instance of Interface type */
UCLASS()
class UNREALDITESTS_API UNeedInterfaceInstance : public UObject
{
    GENERATED_BODY()
public:
    void InitDependencies(TScriptInterface<IReader>&& ReaderInterface)
    {
        Instance = MoveTemp(ReaderInterface);
    }

    TScriptInterface<IReader> Instance;
};

/* Requests factory of Concrete type */
UCLASS()
class UNREALDITESTS_API UNeedObjectFactory : public UObject
{
    GENERATED_BODY()
public:
    void InitDependencies(TFactory<UMockReader>&& ReaderFactory)
    {
        Factory = MoveTemp(ReaderFactory);
    }

    TFactory<UMockReader> Factory;
};

/* Requests factory of Interface type */
UCLASS()
class UNREALDITESTS_API UNeedInterfaceFactory : public UObject
{
    GENERATED_BODY()
public:
    void InitDependencies(TFactory<IReader>&& ReaderFactory)
    {
        Factory = MoveTemp(ReaderFactory);
    }

    TFactory<IReader> Factory;
};

/* Requests Collection of Concrete types */
UCLASS()
class UNREALDITESTS_API UNeedObjectCollection : public UObject
{
    GENERATED_BODY()
public:
    void InitDependencies(TObjectsCollection<UMockReader>&& ReaderCollection)
    {
        Collection = MoveTemp(ReaderCollection);
    }

    TObjectsCollection<UMockReader> Collection;
};

/* Requests Collection of Interface types */
UCLASS()
class UNREALDITESTS_API UNeedInterfaceCollection : public UObject
{
    GENERATED_BODY()
public:
    void InitDependencies(TObjectsCollection<IReader>&& ReaderCollection)
    {
        Collection = MoveTemp(ReaderCollection);
    }

    TObjectsCollection<IReader> Collection;
};