// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "Containers/Array.h"
#include "Containers/Map.h"
#include "Templates/SharedPointer.h"
#include "UObject/Object.h"
#include "DI/ObjectsCollection.h"

class UObjectContainer;
class IInstanceFactory;

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
        void InitOwner(UObject* InOwner);
        void InitServices();

        UObject* GetOwner() const { return Owner; }

        UObject* Resolve(UClass* Type) const;

        TObjectsCollection<UObject> ResolveAll(UClass* Type) const;

        bool IsRegistered(UClass* Type) const;

        void AddReferencedObjects(FReferenceCollector& Collector);

    private:
        friend class FObjectContainerBuilder;
        template <typename T> friend struct TDependencyResolver;
        friend struct FFactoryCallProxy;

        struct FResolver
        {
            TSoftClassPtr<UObject> EffectiveClass;
            TSharedRef<FLifetimeHandler> LifetimeHandler;
        };

        void AddRegistration(UClass* Interface, TSoftClassPtr<UObject> EffectiveClass, const TSharedRef< FLifetimeHandler >& Lifetime);

        const FResolver* GetResolver(UClass* Type) const;
        IInstanceFactory* FindInstanceFactory(UClass* Type) const;
        UObject* ResolveImpl(const FResolver&) const;

        void AppendObjectsCollection(UClass* Type, UObject**& Data) const;

        UObject* Owner = nullptr;
        UObject* OuterForNewObject = nullptr;
        FRegistrationStorage* ParentStorage = nullptr;

        using FResolversArray = TArray<FResolver, TInlineAllocator<2>>;
        TMap<UClass*, FResolversArray> Registrations;
        TArray<TScriptInterface<IInstanceFactory>> InstanceFactories;
    };
}
