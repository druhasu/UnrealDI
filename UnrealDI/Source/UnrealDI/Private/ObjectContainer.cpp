// Copyright Andrei Sudarikov. All Rights Reserved.

#include "DI/ObjectContainer.h"
#include "DI/ObjectContainerDelegates.h"
#include "DI/ObjectsCollection.h"
#include "DI/Impl/DefaultInstanceFactory.h"
#include "DI/Impl/DependenciesRegistry.h"
#include "DI/Impl/Lifetimes.h"
#include "Algo/Copy.h"

class FLifetimeHandler_AutoCreate : public UnrealDI_Impl::FLifetimeHandler
{
public:
    static FLifetimeHandler_AutoCreate Instance;

    UObject* Get() override { return nullptr; }
    UObject* GetOrCreate(const UObject& Context, FNewObjectFactory InNewObjectFactory) override { return InNewObjectFactory(Context, *Class); }
    void AddReferencedObjects(FReferenceCollector& Collector) override {}

    UClass* Class = nullptr;
};

FObjectContainerDelegates::FOnObjectCreated FObjectContainerDelegates::OnObjectConstructedDelegate;
FObjectContainerDelegates::FOnObjectCreated FObjectContainerDelegates::OnObjectInjectedDelegate;
FObjectContainerDelegates::FOnObjectCreated FObjectContainerDelegates::OnObjectCreatedDelegate;

FLifetimeHandler_AutoCreate FLifetimeHandler_AutoCreate::Instance;
UObjectContainer::FResolver UObjectContainer::AutoCreateResolver{ &FLifetimeHandler_AutoCreate::Instance };

void UObjectContainer::BeginDestroy()
{
    for (UnrealDI_Impl::FLifetimeHandler* Lifetime : AllLifetimes)
    {
        UE_ASSUME(Lifetime);
        delete Lifetime;
    }

    AllLifetimes.Empty();

    Super::BeginDestroy();
}

UObject* UObjectContainer::Resolve(UClass* Type) const
{
    checkf(Type, TEXT("Requested object of null type"));

    const auto [Resolver, Container] = GetResolver<true>(Type);
    return Container->ResolveImpl(*Resolver->Lifetime);
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
    return Resolver != nullptr ? Container->ResolveImpl(*Resolver->Lifetime) : nullptr;
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

TScriptInterface<IInjector> UObjectContainer::GetInjector(UObject* InjectTarget) const
{
    return const_cast<UObjectContainer*>(this);
}

void UObjectContainer::AddRegistration(UClass* Interface, UnrealDI_Impl::FLifetimeHandler* Lifetime)
{
    FResolversArray& Resolvers = Registrations.FindOrAdd(Interface);

    Resolvers.Emplace(FResolver{ Lifetime });
}

void UObjectContainer::FinalizeCreation()
{
    // build inheritance chain
    AppendInheritanceChain(InheritanceChain);

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

    // configure AutoCreate resolver to a given class
    FLifetimeHandler_AutoCreate::Instance.Class = Type;

    return MakeTuple(&AutoCreateResolver, this);
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

UObject* UObjectContainer::ResolveImpl(UnrealDI_Impl::FLifetimeHandler& Lifetime) const
{
    return Lifetime.GetOrCreate(*this, &ThisClass::ConstructObject);
}

template <bool bCheck>
TObjectsCollection<UObject> UObjectContainer::ResolveAllImpl(UClass* Type) const
{
    int32 TotalResolvers = 0;

    // calculate total count, so we can allocate enough memory
    for (UObjectContainer* Container : InheritanceChain)
    {
        const FResolversArray* Resolvers = Container->Registrations.Find(Type);
        TotalResolvers += Resolvers ? Resolvers->Num() : 0;
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

    UObject** Iter = Data; // we need a copy of Data, because we will modify it
    for (UObjectContainer* Container : InheritanceChain)
    {
        if (const FResolversArray* Resolvers = Container->Registrations.Find(Type))
        {
            for (const FResolver& Resolver : *Resolvers)
            {
                *Iter = Container->ResolveImpl(*Resolver.Lifetime);
                ++Iter;
            }
        }
    }

    return TObjectsCollection<UObject>(Data, TotalResolvers);
}

void UObjectContainer::AppendInheritanceChain(TArray<UObjectContainer*>& OutChain)
{
    if (ParentContainer != nullptr)
    {
        ParentContainer->AppendInheritanceChain(OutChain);
    }

    OutChain.Add(this);
}

void UObjectContainer::AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector)
{
    UObjectContainer* Container = (UObjectContainer*)InThis;

    for (auto& Lifetime : Container->AllLifetimes)
    {
        Lifetime->AddReferencedObjects(Collector);
    }

    for (auto& InstanceFactory : Container->InstanceFactories)
    {
        InstanceFactory.AddReferencedObjects(Collector);
    }

    Super::AddReferencedObjects(InThis, Collector);
}

UObject* UObjectContainer::ResolveFromContext(const UObject& Context, UClass& Type)
{
    return static_cast<const UObjectContainer&>(Context).Resolve(&Type);
}

UObject* UObjectContainer::ConstructObject(const UObject& Context, UClass& Type)
{
    const UObjectContainer& OwningContainer = static_cast<const UObjectContainer&>(Context);

    // find appropriate factory for the given InClass
    IInstanceFactory* Factory = OwningContainer.FindInstanceFactory(&Type);
    check(Factory != nullptr);

    // create and initialize instance
    UObject* Result = Factory->Create(OwningContainer.OuterForNewObjects, &Type);
    checkf(Result != nullptr, TEXT("IInstanceFactory must never return nullptr. Check project specific implementation"));

    FObjectContainerDelegates::OnObjectConstructedDelegate.Broadcast(*Result, OwningContainer);

    OwningContainer.Inject(Result);
    FObjectContainerDelegates::OnObjectInjectedDelegate.Broadcast(*Result, OwningContainer);

    Factory->FinalizeCreation(Result);
    FObjectContainerDelegates::OnObjectCreatedDelegate.Broadcast(*Result, OwningContainer);

    return Result;
}
