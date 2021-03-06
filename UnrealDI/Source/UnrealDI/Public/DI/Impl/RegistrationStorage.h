// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "Containers/Array.h"
#include "Containers/Map.h"
#include "Templates/SharedPointer.h"
#include "UObject/Object.h"
#include "DI/ObjectsCollection.h"

class UObjectContainer;

template <typename T> class TFactory;

namespace UnrealDI_Impl
{
    class FLifetimeHandler;
    class FRegistrationConfiguratorBase;
    template <typename T> struct TDependencyResolver;
    template <typename T> class TRegistrationConfigurator_ForType;

    class UNREALDI_API FRegistrationStorage
    {
    public:
        UObject* GetOwner() const { return Owner; }
        UObject* GetOuterForNewObject() const { return OuterForNewObject; }
        UWorld* GetWorld() const { return Owner->GetWorld(); }

    protected:
        friend class FObjectContainerBuilder;
        template <typename T> friend struct TDependencyResolver;
        template <typename T> friend class ::TFactory;

        struct FResolver
        {
            FName Name;
            TSharedRef<FLifetimeHandler> LifetimeHandler;
        };

        void InitStorage(UObject* InOwner);

        template <typename T>
        UObject* ResolveImpl();

        TObjectsCollection<UObject> ResolveAll(UClass* Type);

        template <typename TTypeToRegister>
        typename TEnableIf< TIsDerivedFrom< TTypeToRegister, UObject >::Value, TSharedRef< FLifetimeHandler > >::Type
        TryAutoRegisterType(UClass* Type);

        template <typename TTypeToRegister>
        typename TEnableIf< !TIsDerivedFrom< TTypeToRegister, UObject >::Value, TSharedRef< FLifetimeHandler > >::Type
        TryAutoRegisterType(UClass* Type);

        void AddRegistration(UClass* Interface, const TSharedRef< FLifetimeHandler >& Lifetime);

        FResolver* FindResolver(UClass* Type);
        void AppendObjectsCollection(UClass* Type, UObject**& Data);

        UObject* Owner = nullptr;
        UObject* OuterForNewObject = nullptr;
        UObjectContainer* ParentContainer = nullptr;

        TMap<UClass*, TArray<FResolver>> Registrations;
    };
}

#include "DI/Impl/RegistrationConfigurator_ForType.h"

template <typename T>
UObject* UnrealDI_Impl::FRegistrationStorage::ResolveImpl()
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
typename TEnableIf< TIsDerivedFrom< TTypeToRegister, UObject >::Value, TSharedRef< UnrealDI_Impl::FLifetimeHandler > >::Type
UnrealDI_Impl::FRegistrationStorage::TryAutoRegisterType(UClass* Type)
{
    // add registration with default lifetime for TTypeToRegister
    UnrealDI_Impl::TRegistrationConfigurator_ForType<TTypeToRegister> Configurator;
    TSharedRef<UnrealDI_Impl::FLifetimeHandler> Lifetime = Configurator.CreateLifetimeHandler();

    AddRegistration(Type, Lifetime);

    return Lifetime;
}

template <typename TTypeToRegister>
typename TEnableIf< !TIsDerivedFrom< TTypeToRegister, UObject >::Value, TSharedRef< UnrealDI_Impl::FLifetimeHandler > >::Type
UnrealDI_Impl::FRegistrationStorage::TryAutoRegisterType(UClass* Type)
{
    // this overload is for types that cannot be auto registered
    checkf(false, TEXT("Type %s is not registered and may not be auto registered. Only types derived from UObject may be auto registered"), *Type->GetName());

    static TSharedRef<UnrealDI_Impl::FLifetimeHandler> Empty = MakeShared<UnrealDI_Impl::FLifetimeHandler_Instance>(nullptr);
    return Empty;
}