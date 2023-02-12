// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "DI/Impl/InstanceFactoryInvoker.h"
#include "UObject/Object.h"

namespace UnrealDI_Impl
{
    class FRegistrationStorage;

    class FLifetimeHandler
    {
    public:
        virtual ~FLifetimeHandler() = default;

        virtual UObject* Get(FRegistrationStorage& Resolver) = 0;
        virtual void AddReferencedObjects(FReferenceCollector& Collector) = 0;
    };

    class FLifetimeHandler_Transient : public FLifetimeHandler
    {
    public:
        FLifetimeHandler_Transient(FInstanceFactoryInvoker&& Factory)
            : Factory(MoveTemp(Factory))
        {
        }

        UObject* Get(FRegistrationStorage& Resolver) override { return Factory.Invoke(Resolver); }
        void AddReferencedObjects(FReferenceCollector& Collector) override {}

    private:
        FInstanceFactoryInvoker Factory;
    };

    class FLifetimeHandler_CustomFactory : public FLifetimeHandler
    {
    public:
        FLifetimeHandler_CustomFactory(TFunction<UObject* ()> Factory)
            : Factory(Factory)
        {
        }

        UObject* Get(FRegistrationStorage& Resolver) override { return Factory(); }
        void AddReferencedObjects(FReferenceCollector& Collector) override {}

    private:
        TFunction<UObject* ()> Factory;
    };

    class FLifetimeHandler_Instance : public FLifetimeHandler
    {
    public:
        FLifetimeHandler_Instance(UObject* Instance)
            : Instance(Instance)
        {
        }

        UObject* Get(FRegistrationStorage& Resolver) override { return Instance; }
        void AddReferencedObjects(FReferenceCollector& Collector) override
        {
            Collector.AddReferencedObject(Instance);
        }

    private:
        UObject* Instance;
    };

    class FLifetimeHandler_SingleInstance : public FLifetimeHandler
    {
    public:
        FLifetimeHandler_SingleInstance(FInstanceFactoryInvoker&& Factory)
            : Factory(MoveTemp(Factory))
        {
        }

        UObject* Get(FRegistrationStorage& Resolver) override { return Instance ? Instance : (Instance = Factory.Invoke(Resolver)); }
        void AddReferencedObjects(FReferenceCollector& Collector) override
        {
            Collector.AddReferencedObject(Instance);
        }

    private:
        UObject* Instance = nullptr;
        FInstanceFactoryInvoker Factory;
    };

    class FLifetimeHandler_WeakSingleInstance : public FLifetimeHandler
    {
    public:
        FLifetimeHandler_WeakSingleInstance(FInstanceFactoryInvoker&& Factory)
            : Factory(MoveTemp(Factory))
        {
        }

        UObject* Get(FRegistrationStorage& Resolver) override { return Instance.IsValid() ? Instance.Get() : (Instance = Factory.Invoke(Resolver)).Get(); }
        void AddReferencedObjects(FReferenceCollector& Collector) override {}

    private:
        TWeakObjectPtr<UObject> Instance = nullptr;
        FInstanceFactoryInvoker Factory;
    };
}
