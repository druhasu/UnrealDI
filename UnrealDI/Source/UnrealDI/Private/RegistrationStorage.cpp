// Copyright Andrei Sudarikov. All Rights Reserved.

#include "DI/Impl/RegistrationStorage.h"
#include "DI/ObjectContainer.h"

using namespace UnrealDI_Impl;

void FRegistrationStorage::InitStorage(UObject* InOwner)
{
    Owner = InOwner;

    OuterForNewObject = nullptr;
    for (UObject* NextOuter = Owner->GetOuter(); OuterForNewObject == nullptr && NextOuter != nullptr; NextOuter = NextOuter->GetOuter())
    {
        if (NextOuter->IsA<UWorld>() || NextOuter->IsA<UGameInstance>())
        {
            OuterForNewObject = NextOuter;
        }
    }

    if (OuterForNewObject == nullptr)
    {
        OuterForNewObject = GetTransientPackage();
    }
}

TObjectsCollection<UObject> FRegistrationStorage::ResolveAll(UClass* Type)
{
    checkf(Type, TEXT("Requested object of null type"));

    FRegistrationStorage* Container = this;
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

void FRegistrationStorage::AddRegistration(UClass* Interface, const TSharedRef<FLifetimeHandler>& Lifetime)
{
    TArray<FResolver>& Resolvers = Registrations.FindOrAdd(Interface);

    FResolver NewResolver{ FName(NAME_None), Lifetime };

    Resolvers.Emplace(NewResolver);
}

FRegistrationStorage::FResolver* FRegistrationStorage::FindResolver(UClass* Type)
{
    TArray<FResolver>* Resolvers = Registrations.Find(Type);

    if (Resolvers)
    {
        return &Resolvers->Last();
    }
    else if (ParentContainer)
    {
        return ParentContainer->FindResolver(Type);
    }
    else
    {
        // will be checked later in call site
        return nullptr;
    }
}

void FRegistrationStorage::AppendObjectsCollection(UClass* Type, UObject**& Data)
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