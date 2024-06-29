// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "DI/Factory.h"
#include "DI/ObjectsCollection.h"
#include "TestDependency.h"
#include "MockClasses.generated.h"

class IReader;
class UMockReader;

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
    void InitDependencies(TObjectPtr<UMockReader> ReaderConcrete)
    {
        Instance = ReaderConcrete;
    }

    TObjectPtr<UMockReader> Instance;
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

/* Requests custom dependency type */
UCLASS()
class UNeedTestDependency : public UObject
{
    GENERATED_BODY()

public:
    void InitDependencies(TTestDependency<UMockReader> InReader)
    {
        Reader = InReader.Object;
    }

    UMockReader* Reader;
};

/* Requests optional instance of Interface type */
UCLASS()
class UNREALDITESTS_API UNeedOptionalInterfaceInstance : public UObject
{
    GENERATED_BODY()
public:
    void InitDependencies(TOptional<TScriptInterface<IReader>>&& ReaderInterface)
    {
        Instance = ReaderInterface;
    }

    TOptional<TScriptInterface<IReader>> Instance;
};

/* Requests optional factory of Interface type */
UCLASS()
class UNREALDITESTS_API UNeedOptionalInterfaceFactory : public UObject
{
    GENERATED_BODY()
public:
    void InitDependencies(TOptional<TFactory<IReader>>&& ReaderFactory)
    {
        Factory = MoveTemp(ReaderFactory);
    }

    TOptional<TFactory<IReader>> Factory;
};

/* Requests optional Collection of Interface types */
UCLASS()
class UNREALDITESTS_API UNeedOptionalInterfaceCollection : public UObject
{
    GENERATED_BODY()
public:
    void InitDependencies(TOptional<TObjectsCollection<IReader>>&& ReaderCollection)
    {
        Collection = MoveTemp(ReaderCollection);
    }

    TOptional<TObjectsCollection<IReader>> Collection;
};
