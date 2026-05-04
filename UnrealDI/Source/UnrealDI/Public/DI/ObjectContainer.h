// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "IResolver.h"
#include "IInjector.h"
#include "IInjectorProvider.h"
#include "DI/ObjectContainerIterator.h"
#include "ObjectContainer.generated.h"

class IInstanceFactory;

namespace UnrealDI_Impl
{
    class FLifetimeHandler;
    class FObjectContainerIteratorBase;
}

/*
 * Controls which objects to return during iteration
 */
enum class EObjectContainerIteratorFlags : uint8
{
    /* Return objects only in current container */
    None = 0,

    /* Return objects in current container and all of its parents.
     * Objects from parent containers are returned first
     */
    IncludeParent = 1 << 0,
};

UCLASS()
class UNREALDI_API UObjectContainer : public UObject, public IResolver, public IInjector, public IInjectorProvider
{
    GENERATED_BODY()

public:
    // ~Begin UObject overrides
    void BeginDestroy() override;
    // ~End UObject overrides

    // ~Begin IResolver interface
    UObject* Resolve(UClass* Type) const override;
    TObjectsCollection<UObject> ResolveAll(UClass* Type) const override;
    TFactory<UObject> ResolveFactory(UClass* Type) const override;
    UObject* TryResolve(UClass* Type) const override;
    TObjectsCollection<UObject> TryResolveAll(UClass* Type) const override;
    TFactory<UObject> TryResolveFactory(UClass* Type) const override;
    bool IsRegistered(UClass* Type) const override;

    using IResolver::Resolve;
    using IResolver::ResolveAll;
    using IResolver::ResolveFactory;
    using IResolver::TryResolve;
    using IResolver::TryResolveAll;
    using IResolver::TryResolveFactory;
    using IResolver::IsRegistered;
    using IResolver::InvokeWithDependencies;
    // ~End IResolver interface

    // ~Begin IInjector interface
    bool Inject(UObject* Object) const override;
    bool CanInject(UClass* Class) const override;
    // ~End IInjector interface

    // ~Begin IInjectorProvider interface
    TScriptInterface<IInjector> GetInjector(UObject* InjectTarget) const override;
    // ~End IInjectorProvider interface

    /*
     * Returns iterator that allows you to get access to all objects of class T registered in the Container.
     * T may be either subclass of UObject or IInterface.
     * @param Flags - Select which objects should be returned
     */
    template <typename T>
    TObjectContainerIterator<T> CreateIterator(EObjectContainerIteratorFlags Flags = EObjectContainerIteratorFlags::None) const
    {
        return EnumHasAnyFlags(Flags, EObjectContainerIteratorFlags::IncludeParent) ?
            TObjectContainerIterator<T>(MakeConstArrayView(InheritanceChain)) :
            TObjectContainerIterator<T>(MakeConstArrayView(InheritanceChain).Right(1));
    }

private:
    friend class FObjectContainerBuilder;
    friend class FInjectOnConstruction;
    friend class UnrealDI_Impl::FObjectContainerIteratorBase;

    struct FResolver
    {
        UnrealDI_Impl::FLifetimeHandler* Lifetime;
    };

    void AddRegistration(UClass* Interface, UnrealDI_Impl::FLifetimeHandler* Lifetime);
    void FinalizeCreation();

    template <bool bCheck>
    TTuple<const FResolver*, const UObjectContainer*> GetResolver(UClass* Type) const;
    TTuple<const FResolver*, const UObjectContainer*> FindResolver(UClass* Type) const;
    IInstanceFactory* FindInstanceFactory(UClass* Type) const;
    UObject* ResolveImpl(UnrealDI_Impl::FLifetimeHandler& Lifetime) const;
    template <bool bCheck>
    TObjectsCollection<UObject> ResolveAllImpl(UClass* Type) const;

    void AppendInheritanceChain(TArray<UObjectContainer*>& OutChain);

    static void AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector);

    static UObject* ResolveFromContext(const UObject& Context, UClass& Type);
    static UObject* ConstructObject(const UObject& Context, UClass& Type);

    UPROPERTY()
    TObjectPtr<UObject> OuterForNewObjects = nullptr;

    UPROPERTY()
    TObjectPtr<UObjectContainer> ParentContainer = nullptr;

    TArray<UnrealDI_Impl::FLifetimeHandler*> AllLifetimes;

    using FResolversArray = TArray<FResolver, TInlineAllocator<2>>;
    TMap<UClass*, FResolversArray> Registrations;

    TArray<TScriptInterface<IInstanceFactory>, TInlineAllocator<4>> InstanceFactories;

    TArray<UObjectContainer*> InheritanceChain; // container chain starting from most parent to this one

    static FResolver AutoCreateResolver;
};
