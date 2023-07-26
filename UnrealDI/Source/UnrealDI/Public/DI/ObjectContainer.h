// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "IResolver.h"
#include "IInjector.h"
#include "DI/Impl/RegistrationStorage.h"
#include "ObjectContainer.generated.h"

template <typename T> class TObjectsCollection;

// we need this because UHT does not allow inheriting class from namespace
using FRegistrationStorageType = UnrealDI_Impl::FRegistrationStorage;

UCLASS()
class UNREALDI_API UObjectContainer : public UObject, public IResolver, public IInjector, public FRegistrationStorageType
{
    GENERATED_BODY()

public:
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

private:
    friend class FObjectContainerBuilder;

    static void AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector);
};