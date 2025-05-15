// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "IResolver.h"
#include "IInjector.h"
#include "IInjectorProvider.h"
#include "DI/Impl/InvokeWithDependencies.h"
#include "ObjectContainer.generated.h"

class IInstanceFactory;

namespace UnrealDI_Impl
{
    class FLifetimeHandler;
}

UCLASS()
class UNREALDI_API UObjectContainer : public UObject, public IResolver, public IInjector, public IInjectorProvider
{
    GENERATED_BODY()

public:
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
    // ~End IResolver interface

    // ~Begin IInjector interface
    bool Inject(UObject* Object) const override;
    bool CanInject(UClass* Class) const override;
    // ~End IInjector interface

    // ~Begin IInjectorProvider interface
    TScriptInterface<IInjector> GetInjector(UObject* InjectTarget) const override;
    // ~End IInjectorProvider interface

    /*
     * Invokes provided function injecting dependencies into its arguments the same way InitDependencies are usually invoked
     * Example:
     *    Container->InvokeWithDependencies([](UMyService* Service, TScriptInterface<IMyOtherService>&& OtherService)
     *    {  // Do Something with dependencies  });
     */
    template <typename TFunction>
    void InvokeWithDependencies(TFunction&& Function)
    {
        UnrealDI_Impl::TFunctionWithDependenciesInvokerProvider<TFunction>::Invoker::Invoke(*this, Forward<TFunction>(Function));
    }

private:
    friend class FObjectContainerBuilder;
    friend class FInjectOnConstruction;

    struct FResolver
    {
        TSoftClassPtr<UObject> EffectiveClass;
        TSharedRef<UnrealDI_Impl::FLifetimeHandler> LifetimeHandler;
    };

    void AddRegistration(UClass* Interface, TSoftClassPtr<UObject> EffectiveClass, const TSharedRef< UnrealDI_Impl::FLifetimeHandler >& Lifetime);
    void InitServices();

    template <bool bCheck>
    TTuple<const FResolver*, const UObjectContainer*> GetResolver(UClass* Type) const;
    TTuple<const FResolver*, const UObjectContainer*> FindResolver(UClass* Type) const;
    IInstanceFactory* FindInstanceFactory(UClass* Type) const;
    static UObject* ResolveImpl(const FResolver& Resolver, const UObjectContainer* OwningContainer);
    template <bool bCheck>
    TObjectsCollection<UObject> ResolveAllImpl(UClass* Type) const;

    void AppendObjectsCollection(UClass* Type, UObject**& Data) const;

    static void AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector);

    static UObject* ResolveFromContext(const UObject& Context, UClass& Type);

    UPROPERTY()
    TObjectPtr<UObject> OuterForNewObjects = nullptr;

    UPROPERTY()
    TObjectPtr<UObjectContainer> ParentContainer = nullptr;

    using FResolversArray = TArray<FResolver, TInlineAllocator<2>>;
    TMap<UClass*, FResolversArray> Registrations;
    TArray<TScriptInterface<IInstanceFactory>, TInlineAllocator<4>> InstanceFactories;
};
