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

    struct FResolver
    {
        FName Name;
        TSharedPtr<UnrealDI_Impl::FLifetimeHandler> LifetimeHandler;
    };

    template <typename T>
    UObject* ResolveImpl()
    {
        UClass* Type = UnrealDI_Impl::TStaticClass<T>::StaticClass();
        FResolver* Resolver = FindResolver(Type);

        if (Resolver == nullptr)
        {
            // auto-register type T if no registration found for it
            return TryAutoRegisterType<T>(Type)->Get(*this);
        }
        else
        {
            return Resolver->LifetimeHandler->Get(*this);
        }
    }

    template <typename TTypeToRegister>
    typename TEnableIf< TIsDerivedFrom< TTypeToRegister, UObject >::Value, TSharedPtr< UnrealDI_Impl::FLifetimeHandler > >::Type
        TryAutoRegisterType(UClass* Type)
    {
        // add registration with default lifetime for TTypeToRegister
        UnrealDI_Impl::TRegistrationConfigurator_ForType<TTypeToRegister> Configurator;
        TSharedPtr<UnrealDI_Impl::FLifetimeHandler> Lifetime = Configurator.CreateLifetimeHandler();

        AddRegistration(Type, Lifetime);

        return Lifetime;
    }

    template <typename TTypeToRegister>
    typename TEnableIf< !TIsDerivedFrom< TTypeToRegister, UObject >::Value, TSharedPtr< UnrealDI_Impl::FLifetimeHandler > >::Type
        TryAutoRegisterType(UClass* Type)
    {
        // this overload is for types that cannot be auto registered
        checkf(false, TEXT("Type %s is not registered and may not be auto registered. Only types derived from UObject may be auto registered"), *Type->GetName());
        return nullptr;
    }

    FResolver* FindResolver(UClass* Type);
    void AppendObjectsCollection(UClass* Type, UObject**& Data);
    void AddRegistration(UClass* Interface, TSharedPtr<UnrealDI_Impl::FLifetimeHandler> Lifetime);
    static void AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector);

    TMap<UClass*, TArray<FResolver>> Registrations;

    UPROPERTY()
    UObjectContainer* ParentContainer = nullptr;
};