// Copyright Andrei Sudarikov. All Rights Reserved.

#include "DI/Impl/RegistrationStorage.h"
#include "DI/ObjectContainer.h"
#include "DI/Impl/InstanceFactories.h"
#include "DI/Impl/Lifetimes.h"
#include "DI/Impl/DependenciesRegistry.h"
#include "Algo/Copy.h"

namespace UnrealDI_Impl
{

void FRegistrationStorage::InitOwner(UObject* InOwner)
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

void FRegistrationStorage::InitServices()
{
    InstanceFactories.Add(NewObject<UInstanceFactory_Object>(OuterForNewObject));
    InstanceFactories.Add(NewObject<UInstanceFactory_Actor>(OuterForNewObject));
    InstanceFactories.Add(NewObject<UInstanceFactory_UserWidget>(OuterForNewObject));

    if (Registrations.Contains(UInstanceFactory::StaticClass()))
    {
        Algo::Copy(ResolveAll(UInstanceFactory::StaticClass()), InstanceFactories);
    }

    Algo::Reverse(InstanceFactories);
}

UObject* FRegistrationStorage::Resolve(UClass* Type) const
{
    const FResolver* Resolver = GetResolver(Type);
    return ResolveImpl(*Resolver);
}

TObjectsCollection<UObject> FRegistrationStorage::ResolveAll(UClass* Type) const
{
    checkf(Type, TEXT("Requested object of null type"));

    const FRegistrationStorage* Container = this;
    int32 TotalResolvers = 0;

    // calculate total count, so we can allocate enough memory
    while (Container)
    {
        const FResolversArray* Resolvers = Container->Registrations.Find(Type);
        TotalResolvers += Resolvers ? Resolvers->Num() : 0;
        Container = Container->ParentStorage;
    }

    // if no types were registered, it's probably not what was expected
    checkf(TotalResolvers > 0, TEXT("Type %s is not registered"), *Type->GetName());

    // Data will be owned by TObjectsCollection and freed by it
    UObject** Data = (UObject**)FMemory::Malloc(TotalResolvers * sizeof(UObject*));

    UObject** Iter = Data; // we need a copy of Data, because AppendObjectsCollection will modify it
    AppendObjectsCollection(Type, Iter);

    return TObjectsCollection<UObject>(Data, TotalResolvers);
}

bool FRegistrationStorage::IsRegistered(UClass* Type) const
{
    if (Registrations.Contains(Type))
    {
        return true;
    }
    else if (ParentStorage)
    {
        return ParentStorage->IsRegistered(Type);
    }

    return false;
}

void FRegistrationStorage::AddReferencedObjects(FReferenceCollector& Collector)
{
    for (auto& Resolvers : Registrations)
    {
        for (FResolver& Resolver : Resolvers.Value)
        {
            Resolver.LifetimeHandler->AddReferencedObjects(Collector);
        }
    }

    for (auto& InstanceFactory : InstanceFactories)
    {
        InstanceFactory.AddReferencedObjects(Collector);
    }

    if (ParentStorage != nullptr)
    {
        Collector.AddReferencedObject(ParentStorage->Owner);
    }
}

void FRegistrationStorage::AddRegistration(UClass* Interface, TSoftClassPtr<UObject> EffectiveClass, const TSharedRef<FLifetimeHandler>& Lifetime)
{
    FResolversArray& Resolvers = Registrations.FindOrAdd(Interface);

    Resolvers.Emplace(FResolver{ MoveTemp(EffectiveClass), Lifetime });
}

const FRegistrationStorage::FResolver* FRegistrationStorage::GetResolver(UClass* Type) const
{
    const FResolversArray* Resolvers = Registrations.Find(Type);

    if (Resolvers)
    {
        return &Resolvers->Last();
    }
    else if (ParentStorage)
    {
        return ParentStorage->GetResolver(Type);
    }
    else
    {
        // auto-register Type if no registration found for it

        if (Type->IsChildOf<UInterface>())
        {
            checkf(!"Cannot auto register type", TEXT("Type %s is not registered and may not be auto registered. Only types derived from UObject may be auto registered"), *Type->GetName());
            return nullptr;
        }

        FResolversArray& NewArray = const_cast<FRegistrationStorage*>(this)->Registrations.Emplace(Type, { FResolver { Type, MakeShared<FLifetimeHandler_Transient>() } });

        return &NewArray.Last();
    }
}

IInstanceFactory* FRegistrationStorage::FindInstanceFactory(UClass* Type) const
{
    for (auto& InstanceFactory : InstanceFactories)
    {
        if (InstanceFactory->IsClassSupported(Type))
        {
            return InstanceFactory.GetInterface();
        }
    }

    // should never happen
    return nullptr;
}

UObject* FRegistrationStorage::ResolveImpl(const FResolver& Resolver) const
{
    UObject* Result = Resolver.LifetimeHandler->Get();
    if (Result == nullptr)
    {
        UClass* EffectiveClass = Resolver.EffectiveClass.LoadSynchronous();
        check(EffectiveClass != nullptr);

        // create and initialize instance
        IInstanceFactory* Factory = FindInstanceFactory(EffectiveClass);
        check(Factory != nullptr);

        Result = Factory->Create(OuterForNewObject, EffectiveClass);
        checkf(Result != nullptr, TEXT("IInstanceFactory must never return nullptr. Check project specific implementation"));

        if (auto Func = FDependenciesRegistry::FindInitFunction(EffectiveClass))
        {
            Func(*Result, *this);
        }

        Factory->FinalizeCreation(Result);

        Resolver.LifetimeHandler->Set(Result);
    }

    return Result;
}

void FRegistrationStorage::AppendObjectsCollection(UClass* Type, UObject**& Data) const
{
    if (ParentStorage)
    {
        ParentStorage->AppendObjectsCollection(Type, Data);
    }

    const FResolversArray* Resolvers = Registrations.Find(Type);
    if (Resolvers)
    {
        for (const FResolver& Resolver : *Resolvers)
        {
            *Data = ResolveImpl(Resolver);
            ++Data;
        }
    }
}

}
