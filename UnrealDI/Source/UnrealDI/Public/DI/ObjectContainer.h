// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "IResolver.h"
#include "IInjector.h"
#include "DI/Impl/RegistrationStorage.h"
#include "DI/Impl/InvokeWithDependencies.h"
#include "ObjectContainer.generated.h"

template <typename T> class TObjectsCollection;

UCLASS()
class UNREALDI_API UObjectContainer : public UObject, public IResolver, public IInjector
{
    GENERATED_BODY()

public:

    void PostInitProperties() override;

    // ~Begin IResolver interface
    UObject* Resolve(class UClass* Type) override;
    TObjectsCollection<UObject> ResolveAll(UClass* Type) override;
    bool IsRegistered(class UClass* Type) override;

    using IResolver::Resolve;
    using IResolver::ResolveAll;
    using IResolver::IsRegistered;
    // ~End IResolver interface

    // ~Begin IInjector interface
    bool Inject(UObject* Object) const override;
    bool CanInject(UClass* Class) const override;
    // ~End IInjector interface

    /*
     * Invokes provided function injecting dependencies into its arguments the same way InitDependencies are usually invoked
     * Example:
     *    Container->InvokeWithDependencies([](UMyService* Service, TScriptInterface<IMyOtherService>&& OtherService)
     *    {  // Do Something with dependencies  });
     */
    template <typename TFunction>
    void InvokeWithDependencies(TFunction&& Function)
    {
        UnrealDI_Impl::TFunctionWithDependenciesInvokerProvider<TFunction>::Invoker::Invoke(Storage, Forward<TFunction>(Function));
    }

private:
    friend class FObjectContainerBuilder;
    friend class FInjectOnConstruction;

    static void AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector);

    UnrealDI_Impl::FRegistrationStorage Storage;
};
