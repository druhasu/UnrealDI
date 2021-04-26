// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "IResolver.h"
#include "DI/ObjectsCollection.h"
#include "DI/Impl/RegistrationConfigurator_ForType.h"
#include "ObjectContainer.generated.h"

namespace UnrealDI_Impl
{
    class FLifetimeHandler;
    class FRegistrationConfiguratorBase;
    template <typename T> struct TDependencyResolver;
}

UCLASS()
class UNREALDI_API UObjectContainer : public UObject, public IResolver
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

private:
    friend class FObjectContainerBuilder;
    template <typename T> friend struct UnrealDI_Impl::TDependencyResolver;
    template <typename T> friend class TFactory;

    template <typename T>
    UObject* ResolveImpl()
    {
        UClass* Key = UnrealDI_Impl::TStaticClass<T>::StaticClass();
        TArray<FResolver>* Resolvers = Registrations.Find(Key);

        if (Resolvers == nullptr)
        {
            // auto-register type T if no registration found for it
            return TryAutoRegisterType<T>()->Get(*this);
        }
        else
        {
            return Resolvers->Last().LifetimeHandler->Get(*this);
        }
    }

    template <typename T>
    typename TEnableIf< TIsDerivedFrom< T, UObject >::Value, TSharedPtr< UnrealDI_Impl::FLifetimeHandler > >::Type
        TryAutoRegisterType()
    {
        // add registration with default lifetime for T
        UnrealDI_Impl::TRegistrationConfigurator_ForType<T> Configurator;
        TSharedPtr<UnrealDI_Impl::FLifetimeHandler> Lifetime = Configurator.GetLifetimeHandler();

        UClass* Key = UnrealDI_Impl::TStaticClass<T>::StaticClass();
        AddRegistration(Key, Lifetime);

        return Lifetime;
    }

    template <typename T>
    typename TEnableIf< !TIsDerivedFrom< T, UObject >::Value, TSharedPtr< UnrealDI_Impl::FLifetimeHandler > >::Type
        TryAutoRegisterType()
    {
        // this overload is for types that cannot be auto registered
        checkf(false, TEXT("Only types derived from UObject may be auto registered"));
        return nullptr;
    }

    void AddRegistration(UClass* Interface, TSharedPtr<UnrealDI_Impl::FLifetimeHandler> Lifetime);

    static void AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector);
    
    struct FResolver
    {
        FName Name;
        TSharedPtr<UnrealDI_Impl::FLifetimeHandler> LifetimeHandler;
    };

    TMap<UClass*, TArray<FResolver>> Registrations;
};