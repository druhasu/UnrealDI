// Copyright Andrei Sudarikov. All Rights Reserved.

#include "DI/ObjectContainer.h"
#include "DI/ObjectsCollection.h"

void UObjectContainer::PostInitProperties()
{
    Super::PostInitProperties();

    Storage.InitOwner(this);
}

UObject* UObjectContainer::Resolve(UClass* Type) const
{
    checkf(Type, TEXT("Requested object of null type"));

    return Storage.Resolve(Type);
}

TObjectsCollection<UObject> UObjectContainer::ResolveAll(UClass* Type) const
{
    checkf(Type, TEXT("Requested object of null type"));

    return Storage.ResolveAll(Type);
}

bool UObjectContainer::IsRegistered(UClass* Type) const
{
    checkf(Type, TEXT("Requested object of null type"));

    return Storage.IsRegistered(Type);
}

bool UObjectContainer::Inject(UObject* Object) const
{
    check(Object);

    return Storage.Inject(Object);
}

bool UObjectContainer::CanInject(UClass* Class) const
{
    check(Class);

    return Storage.CanInject(Class);
}

void UObjectContainer::AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector)
{
    UObjectContainer* Container = (UObjectContainer*)InThis;

    Container->Storage.AddReferencedObjects(Collector);
}
