// Copyright Andrei Sudarikov. All Rights Reserved.

#include "DI/ObjectContainer.h"
#include "DI/Impl/Lifetimes.h"
#include "DI/Impl/RegistrationConfiguratorBase.h"

UObject* UObjectContainer::Resolve(UClass* Type)
{
    checkf(Type, TEXT("Requested object of null type"));

    FResolver* Resolver = FindResolver(Type);
    checkf(Resolver, TEXT("Type %s is not registered"), *Type->GetName());

    return Resolver->LifetimeHandler->Get(*this);
}

bool UObjectContainer::IsRegistered(UClass* Type)
{
    return Registrations.Find(Type) != nullptr;
}

TObjectsCollection<UObject> UObjectContainer::ResolveAll(UClass* Type)
{
    checkf(Type, TEXT("Requested object of null type"));

    UObjectContainer* Container = this;
    int32 TotalResolvers = 0;

    // calculate total count, so we can allocate enough memory
    while (Container)
    {
        TArray<FResolver>* Resolvers = Container->Registrations.Find(Type);
        TotalResolvers += Resolvers ? Resolvers->Num() : 0;
        Container = Container->ParentContainer;
    }

    // if no types were registered, it's probably not what was expected
    checkf(TotalResolvers > 0, TEXT("Type %s is not registered"), *Type->GetName());

    // Data will be owned by TObjectsCollection and freed by it
    UObject** Data = (UObject**)FMemory::Malloc(TotalResolvers * sizeof(UObject*));

    UObject** Iter = Data; // we need a copy of Data, because AppendObjectsCollection will modify it
    AppendObjectsCollection(Type, Iter);

    return TObjectsCollection<UObject>(Data, TotalResolvers);
}

UObjectContainer::FResolver* UObjectContainer::FindResolver(UClass* Type)
{
    TArray<FResolver>* Resolvers = Registrations.Find(Type);

    if (Resolvers)
    {
        return &Resolvers->Last();
    }
    else if(ParentContainer)
    {
        return ParentContainer->FindResolver(Type);
    }
    else
    {
        // will be checked later in call site
        return nullptr;
    }
}

void UObjectContainer::AppendObjectsCollection(UClass* Type, UObject**& Data)
{
    if (ParentContainer)
    {
        ParentContainer->AppendObjectsCollection(Type, Data);
    }

    TArray<FResolver>* Resolvers = Registrations.Find(Type);
    if (Resolvers)
    {
        for (const FResolver& Resolver : *Resolvers)
        {
            *Data = Resolver.LifetimeHandler->Get(*this);
            ++Data;
        }
    }
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