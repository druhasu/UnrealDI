// Copyright Andrei Sudarikov. All Rights Reserved.

#include "DI/ObjectContainer.h"
#include "DI/Impl/Lifetimes.h"
#include "DI/Impl/RegistrationConfiguratorBase.h"

UObject* UObjectContainer::Resolve(UClass* Type)
{
    checkf(Type, TEXT("Requested object of null type"));

    TArray<FResolver>* Resolvers = Registrations.Find(Type);
    checkf(Resolvers, TEXT("Type %s not registered"), *Type->GetName());

    return Resolvers->Last().LifetimeHandler->Get(*this);
}

bool UObjectContainer::IsRegistered(UClass* Type)
{
    return Registrations.Find(Type) != nullptr;
}

TObjectsCollection<UObject> UObjectContainer::ResolveAll(UClass* Type)
{
    checkf(Type, TEXT("Requested object of null type"));

    TArray<FResolver>* Resolvers = Registrations.Find(Type);
    checkf(Resolvers, TEXT("Type %s not registered"), *Type->GetName());

    // Data will be owned by TObjectsCollection and freed by it
    UObject** Data = (UObject**)FMemory::Malloc(Resolvers->Num() * sizeof(UObject*));
    int32 Counter = 0;

    for (const FResolver& Resolver : *Resolvers)
    {
        Data[Counter] = Resolver.LifetimeHandler->Get(*this);
        ++Counter;
    }

    return TObjectsCollection<UObject>(Data, Resolvers->Num());
}

void UObjectContainer::AddRegistration(UClass* Interface, TSharedPtr<UnrealDI_Impl::FLifetimeHandler> Lifetime)
{
    TArray<FResolver>& Resolvers = Registrations.FindOrAdd(Interface);

    UObjectContainer::FResolver NewResolver;
    NewResolver.LifetimeHandler = Lifetime;
 
    Resolvers.Emplace(NewResolver);
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
}