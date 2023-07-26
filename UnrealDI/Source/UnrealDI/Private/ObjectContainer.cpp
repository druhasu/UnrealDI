// Copyright Andrei Sudarikov. All Rights Reserved.

#include "DI/ObjectContainer.h"
#include "DI/Impl/Lifetimes.h"
#include "DI/Impl/RegistrationConfiguratorBase.h"
#include "DI/Impl/DependenciesRegistry.h"
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

bool UObjectContainer::Inject(UObject* Object) const
{
    check(Object);

    UClass* Class = Object->GetClass();

    auto InitFunction = UnrealDI_Impl::FDependenciesRegistry::FindInitFunction(Class);
    if (InitFunction != nullptr)
    {
        InitFunction(*Object, *const_cast<UObjectContainer*>(this));
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

    for (auto& Resolvers : Container->Registrations)
    {
        for (FResolver& Resolver : Resolvers.Value)
        {
            Resolver.LifetimeHandler->AddReferencedObjects(Collector);
        }
    }

    Collector.AddReferencedObject(Container->ParentContainer);
}