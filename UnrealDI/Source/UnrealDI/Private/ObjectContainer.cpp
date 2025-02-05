// Copyright Andrei Sudarikov. All Rights Reserved.

#include "DI/ObjectContainer.h"
#include "DI/ObjectsCollection.h"
#include "DI/Impl/DefaultInstanceFactory.h"
#include "DI/Impl/DependenciesRegistry.h"
#include "DI/Impl/Lifetimes.h"

UObject* UObjectContainer::Resolve(UClass* Type) const
{
    checkf(Type, TEXT("Requested object of null type"));

    const auto [Resolver, Container] = GetResolver<true>(Type);
    return ResolveImpl(*Resolver, Container);
}

TObjectsCollection<UObject> UObjectContainer::ResolveAll(UClass* Type) const
{
    checkf(Type, TEXT("Requested object of null type"));

    return ResolveAllImpl<true>(Type);
}

TFactory<UObject> UObjectContainer::ResolveFactory(UClass* Type) const
{
    checkf(Type, TEXT("Requested object of null type"));

    const auto [Resolver, Container] = GetResolver<true>(Type);
    return TFactory<UObject>(*Container, &ThisClass::ResolveFromContext);
}

UObject* UObjectContainer::TryResolve(UClass* Type) const
{
    checkf(Type, TEXT("Requested object of null type"));

    const auto [Resolver, Container] = GetResolver<false>(Type);
    return Resolver != nullptr ? ResolveImpl(*Resolver, Container) : nullptr;
}

TObjectsCollection<UObject> UObjectContainer::TryResolveAll(UClass* Type) const
{
    checkf(Type, TEXT("Requested object of null type"));

    return ResolveAllImpl<false>(Type);
}

TFactory<UObject> UObjectContainer::TryResolveFactory(UClass* Type) const
{
    checkf(Type, TEXT("Requested object of null type"));

    const auto [Resolver, Container] = GetResolver<false>(Type);
    return Resolver != nullptr ? TFactory<UObject>(*Container, &ThisClass::ResolveFromContext) : TFactory<UObject>();
}

bool UObjectContainer::IsRegistered(UClass* Type) const
{
    checkf(Type, TEXT("Requested object of null type"));

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
    using namespace UnrealDI_Impl;
    check(Object);

    UClass* Class = Object->GetClass();

    FDependenciesRegistry::FInitFunctionPtr NativeInitFunction = nullptr;
    UFunction* BlueprintInitFunction = nullptr;

    FDependenciesRegistry::FindInitFunctions(Class, NativeInitFunction, BlueprintInitFunction);

    // first - call native InitDependencies
    if (NativeInitFunction != nullptr)
    {
        NativeInitFunction(*Object, *static_cast<const IResolver*>(this));
    }

    // then -  call blueprint InitDependencies
    if (BlueprintInitFunction != nullptr)
    {
        uint8* Arguments = (uint8*)FMemory_Alloca(BlueprintInitFunction->ParmsSize);
        FMemory::Memzero(Arguments, BlueprintInitFunction->ParmsSize);

        uint8* CurrentArgument = Arguments;

        // prepare arguments
        for (TFieldIterator<FProperty> It(BlueprintInitFunction, EFieldIterationFlags::None); It; ++It)
        {
            if (It->HasAllPropertyFlags(CPF_Parm))
            {
                if (FObjectProperty* ObjectProperty = CastField<FObjectProperty>(*It))
                {
                    new (CurrentArgument) TObjectPtr<UObject>(Resolve(ObjectProperty->PropertyClass));
                    CurrentArgument += sizeof(TObjectPtr<UObject>);
                }
                else if (FInterfaceProperty* InterfaceProperty = CastField<FInterfaceProperty>(*It))
                {
                    UObject* Result = Resolve(InterfaceProperty->InterfaceClass);
                    new (CurrentArgument) FScriptInterface(Result, Result->GetInterfaceAddress(InterfaceProperty->InterfaceClass));
                    CurrentArgument += sizeof(FScriptInterface);
                }
            }
        }

        check(CurrentArgument - Arguments == BlueprintInitFunction->ParmsSize);

        Object->ProcessEvent(BlueprintInitFunction, Arguments);
    }

    return NativeInitFunction || BlueprintInitFunction;
}

bool UObjectContainer::CanInject(UClass* Class) const
{
    using namespace UnrealDI_Impl;
    check(Class);

    FDependenciesRegistry::FInitFunctionPtr NativeInitFunction = nullptr;
    UFunction* BlueprintInitFunction = nullptr;

    FDependenciesRegistry::FindInitFunctions(Class, NativeInitFunction, BlueprintInitFunction);

    return NativeInitFunction || BlueprintInitFunction;
}

void UObjectContainer::AddRegistration(UClass* Interface, TSoftClassPtr<UObject> EffectiveClass, const TSharedRef<UnrealDI_Impl::FLifetimeHandler>& Lifetime)
{
    FResolversArray& Resolvers = Registrations.FindOrAdd(Interface);

    Resolvers.Emplace(FResolver{ MoveTemp(EffectiveClass), Lifetime });
}

void UObjectContainer::InitServices()
{
    if (ParentContainer == nullptr)
    {
        // no point in creating Default Factory if we have parent container. we can take it from there
        InstanceFactories.Add(GetMutableDefault<UDefaultInstanceFactory>());
    }

    // add user provided factories
    if (Registrations.Contains(UInstanceFactory::StaticClass()))
    {
        Algo::Copy(ResolveAllImpl<false>(UInstanceFactory::StaticClass()), InstanceFactories);
    }

    // order by 'most recently added'
    Algo::Reverse(InstanceFactories);
}

template <bool bCheck>
TTuple<const UObjectContainer::FResolver*, const UObjectContainer*> UObjectContainer::GetResolver(UClass* Type) const
{
    auto ResolverTuple = FindResolver(Type);

    if (ResolverTuple.Key != nullptr)
    {
        return ResolverTuple;
    }

    // make sure that we can auto-register this type
    if (Type->IsChildOf<UInterface>())
    {
        if constexpr (bCheck)
        {
            checkf(!"Cannot auto register type", TEXT("Type %s is not registered and may not be auto registered. Only types derived from UObject may be auto registered"), *Type->GetName());
        }

        return MakeTuple(nullptr, this);
    }

    // auto-register Type if no registration found for it
    FResolversArray& NewArray = const_cast<UObjectContainer*>(this)->Registrations.Emplace(Type, { FResolver { Type, MakeShared<UnrealDI_Impl::FLifetimeHandler_Transient>() } });

    return MakeTuple(&NewArray.Last(), this);
}

TTuple<const UObjectContainer::FResolver*, const UObjectContainer*> UObjectContainer::FindResolver(UClass* Type) const
{
    const FResolversArray* Resolvers = Registrations.Find(Type);

    if (Resolvers)
    {
        return MakeTuple(&Resolvers->Last(), this);
    }
    else if (ParentContainer)
    {
        return ParentContainer->FindResolver(Type);
    }

    return MakeTuple(nullptr, this);
}

IInstanceFactory* UObjectContainer::FindInstanceFactory(UClass* Type) const
{
    for (auto& InstanceFactory : InstanceFactories)
    {
        if (InstanceFactory->IsClassSupported(Type))
        {
            return InstanceFactory.GetInterface();
        }
    }

    // ParentContainer cannot not be null here
    return ParentContainer->FindInstanceFactory(Type);
}

UObject* UObjectContainer::ResolveImpl(const FResolver& Resolver, const UObjectContainer* OwningContainer)
{
    // cache reference to LifetimeHandler, because reference to Resolver may become invalid during call to Inject due to Registrations map memory reallocation
    UnrealDI_Impl::FLifetimeHandler& LifetimeHandler = Resolver.LifetimeHandler.Get();

    UObject* Result = LifetimeHandler.Get();
    if (Result == nullptr)
    {
        UClass* EffectiveClass = Resolver.EffectiveClass.LoadSynchronous();
        check(EffectiveClass != nullptr);

        // create and initialize instance
        IInstanceFactory* Factory = OwningContainer->FindInstanceFactory(EffectiveClass);
        check(Factory != nullptr);

        Result = Factory->Create(OwningContainer->OuterForNewObjects, EffectiveClass);
        checkf(Result != nullptr, TEXT("IInstanceFactory must never return nullptr. Check project specific implementation"));

        OwningContainer->Inject(Result);
        // Resolver may be invalid after this call

        Factory->FinalizeCreation(Result);

        LifetimeHandler.Set(Result);
    }

    return Result;
}

template <bool bCheck>
TObjectsCollection<UObject> UObjectContainer::ResolveAllImpl(UClass* Type) const
{
    const UObjectContainer* Container = this;
    int32 TotalResolvers = 0;

    // calculate total count, so we can allocate enough memory
    while (Container)
    {
        const FResolversArray* Resolvers = Container->Registrations.Find(Type);
        TotalResolvers += Resolvers ? Resolvers->Num() : 0;
        Container = Container->ParentContainer;
    }

    if constexpr (bCheck)
    {
        // if no types were registered, it's probably not what was expected
        checkf(TotalResolvers > 0, TEXT("Type %s is not registered"), *Type->GetName());
    }
    else
    {
        if (TotalResolvers == 0)
        {
            return TObjectsCollection<UObject>();
        }
    }

    // Data will be owned by TObjectsCollection and freed by it
    UObject** Data = (UObject**)FMemory::Malloc(TotalResolvers * sizeof(UObject*));

    UObject** Iter = Data; // we need a copy of Data, because AppendObjectsCollection will modify it
    AppendObjectsCollection(Type, Iter);

    return TObjectsCollection<UObject>(Data, TotalResolvers);
}

void UObjectContainer::AppendObjectsCollection(UClass* Type, UObject**& Data) const
{
    if (ParentContainer)
    {
        ParentContainer->AppendObjectsCollection(Type, Data);
    }

    const FResolversArray* Resolvers = Registrations.Find(Type);
    if (Resolvers)
    {
        for (const FResolver& Resolver : *Resolvers)
        {
            *Data = ResolveImpl(Resolver, this);
            ++Data;
        }
    }
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

    for (auto& InstanceFactory : Container->InstanceFactories)
    {
        InstanceFactory.AddReferencedObjects(Collector);
    }
}

UObject* UObjectContainer::ResolveFromContext(const UObject& Context, UClass& Type)
{
    return static_cast<const UObjectContainer&>(Context).Resolve(&Type);
}
