// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "DI/Impl/InstanceFactory.h"
#include "UObject/Object.h"
#include "Templates/Function.h"

class UObjectContainer;

namespace UnrealDI_Impl
{
    class FLifetimeHandler
    {
    public:
        virtual ~FLifetimeHandler() = default;

        virtual UObject* Get(UObjectContainer& Resolver) = 0;
        virtual void AddReferencedObjects(FReferenceCollector& Collector) = 0;
    };

    class FLifetimeHandler_Transient : public FLifetimeHandler
    {
    public:
        FLifetimeHandler_Transient(FInstanceFactoryCallable&& Factory)
            : Factory(MoveTemp(Factory))
        {
        }

        UObject* Get(UObjectContainer& Resolver) override { return Factory.Function(Resolver); }
        void AddReferencedObjects(FReferenceCollector& Collector) override
        {
            Collector.AddReferencedObject(Factory.BlueprintClass);
        }

    private:
        FInstanceFactoryCallable Factory;
    };

    class FLifetimeHandler_Instance : public FLifetimeHandler
    {
    public:
        FLifetimeHandler_Instance(UObject* Instance)
            : Instance(Instance)
        {
        }

        UObject* Get(UObjectContainer& Resolver) override { return Instance; }
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
        FLifetimeHandler_SingleInstance(FInstanceFactoryCallable&& Factory)
            : Factory(MoveTemp(Factory))
        {
        }

        UObject* Get(UObjectContainer& Resolver) override { return Instance ? Instance : (Instance = Factory.Function(Resolver)); }
        void AddReferencedObjects(FReferenceCollector& Collector) override
        {
            Collector.AddReferencedObject(Instance);
            Collector.AddReferencedObject(Factory.BlueprintClass);
        }

    private:
        UObject* Instance = nullptr;
        FInstanceFactoryCallable Factory;
    };

    class FLifetimeHandler_WeakSingleInstance : public FLifetimeHandler
    {
    public:
        FLifetimeHandler_WeakSingleInstance(FInstanceFactoryCallable&& Factory)
            : Factory(MoveTemp(Factory))
        {
        }

        UObject* Get(UObjectContainer& Resolver) override { return Instance.IsValid() ? Instance.Get() : (Instance = Factory.Function(Resolver)).Get(); }
        void AddReferencedObjects(FReferenceCollector& Collector) override
        {
            Collector.AddReferencedObject(Factory.BlueprintClass);
        }

    private:
        TWeakObjectPtr<UObject> Instance = nullptr;
        FInstanceFactoryCallable Factory;
    };
}
