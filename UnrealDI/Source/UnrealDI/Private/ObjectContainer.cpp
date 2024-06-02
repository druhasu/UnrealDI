// Copyright Andrei Sudarikov. All Rights Reserved.

#include "DI/ObjectContainer.h"
#include "DI/Impl/Lifetimes.h"
#include "DI/Impl/RegistrationConfiguratorBase.h"
#include "DI/Impl/DependenciesRegistry.h"
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

    UClass* Class = Object->GetClass();

    auto InitFunction = UnrealDI_Impl::FDependenciesRegistry::FindInitFunction(Class);
    if (InitFunction != nullptr)
    {
        InitFunction(*Object, *this);
        return true;
    }

    return false;
}

bool UObjectContainer::CanInject(UClass* Class) const
{
    check(Class);

    return UnrealDI_Impl::FDependenciesRegistry::FindInitFunction(Class) != nullptr;
}

void UObjectContainer::AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector)
{
    UObjectContainer* Container = (UObjectContainer*)InThis;

    Container->Storage.AddReferencedObjects(Collector);
}
