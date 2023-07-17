// Copyright Andrei Sudarikov. All Rights Reserved.

#include "DI/ObjectContainer.h"
#include "DI/Impl/Lifetimes.h"
#include "DI/Impl/RegistrationConfiguratorBase.h"
#include "DI/ObjectsCollection.h"


UObject* UObjectContainer::Resolve(UClass* Type)
{
    checkf(Type, TEXT("Requested object of null type"));

    FResolver* Resolver = FindResolver(Type);
    checkf(Resolver, TEXT("Type %s is not registered"), *Type->GetName());

    return Resolver->LifetimeHandler->Get(*this);
}

TObjectsCollection<UObject> UObjectContainer::ResolveAll(UClass* Type)
{
    return FRegistrationStorageType::ResolveAll(Type);
}

bool UObjectContainer::IsRegistered(UClass* Type)
{
    if (Registrations.Contains(Type))
    {
        return true;
    }
    else if (ParentContainer)
    {
        return ParentContainer->IsRegistered(Type);
    }

    return false;
}

bool UObjectContainer::Inject(UObject* Object)
{
    check(Object);

    UClass* Class = Object->GetClass();

    // find native parent, because we store only those in Registrations
    while (!Class->IsNative())
    {
        Class = Class->GetSuperClass();
    }

    // try find an injector for a class
    UnrealDI_Impl::FInstanceInjectorFunction Injector = nullptr;
    while (!Injector && Class)
    {
        Injector = FindInjector(Class);
        Class = Class->GetSuperClass();
    }

    if (Injector)
    {
        Injector(*Object, *this);
        return true;
    }

    return false;
}

void UObjectContainer::AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector)
{
    UObjectContainer* Container = (UObjectContainer*)InThis;

    for (auto& Resolvers : Container->Registrations)
    {
        for (FResolver& Resolver : Resolvers.Value)
        {
            Resolver.LifetimeHandler->AddReferencedObjects(Collector);
        }
    }

    Collector.AddReferencedObject(Container->ParentContainer);
}