// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "UObject/Object.h"

namespace UnrealDI_Impl
{
    class FLifetimeHandler
    {
    public:
        virtual ~FLifetimeHandler() = default;

        using FNewObjectFactory = UObject* (*)(const UObject& Context, UClass& NewObjectClass);

        virtual UObject* Get() = 0;
        virtual UObject* GetOrCreate(const UObject& Context, FNewObjectFactory InNewObjectFactory) = 0;
        virtual void AddReferencedObjects(FReferenceCollector& Collector) = 0;
    };

    class FLifetimeHandler_Transient : public FLifetimeHandler
    {
    public:
        FLifetimeHandler_Transient(TSoftClassPtr<UObject>&& InClass)
            : Class(MoveTemp(InClass))
        {
        }

        UObject* Get() override { return nullptr; }
        UObject* GetOrCreate(const UObject& Context, FNewObjectFactory InNewObjectFactory) override { return InNewObjectFactory(Context, *Class.LoadSynchronous()); }
        void AddReferencedObjects(FReferenceCollector& Collector) override {}

        static FLifetimeHandler* Make(TSoftClassPtr<UObject> InClass) { return new FLifetimeHandler_Transient(MoveTemp(InClass)); }

    private:
        TSoftClassPtr<UObject> Class;
    };

    class FLifetimeHandler_StaticFactory : public FLifetimeHandler
    {
    public:
        using FunctionPtr = UObject* (*)();

        FLifetimeHandler_StaticFactory(FunctionPtr Factory)
            : Factory(Factory)
        {
        }

        UObject* Get() override { return Factory(); }
        UObject* GetOrCreate(const UObject& Context, FNewObjectFactory InNewObjectFactory) override { return Factory(); }
        void AddReferencedObjects(FReferenceCollector& Collector) override {}

    private:
        FunctionPtr Factory;
    };

    class FLifetimeHandler_CustomFactory : public FLifetimeHandler
    {
    public:
        FLifetimeHandler_CustomFactory(TFunction<UObject* ()> Factory)
            : Factory(Factory)
        {
        }

        UObject* Get() override { return Factory(); }
        UObject* GetOrCreate(const UObject& Context, FNewObjectFactory InNewObjectFactory) override { return Factory(); }
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

        UObject* Get() override { return Instance; }
        UObject* GetOrCreate(const UObject& Context, FNewObjectFactory InNewObjectFactory) override { return Instance; }
        void AddReferencedObjects(FReferenceCollector& Collector) override
        {
            Collector.AddReferencedObject(Instance);
        }

    private:
        TObjectPtr<UObject> Instance;
    };

    class FLifetimeHandler_SingleInstance : public FLifetimeHandler
    {
    public:
        FLifetimeHandler_SingleInstance(TSoftClassPtr<UObject>&& InClass)
            : Class(MoveTemp(InClass))
        {
        }

        UObject* Get() override { return Instance; }
        UObject* GetOrCreate(const UObject& Context, FNewObjectFactory InNewObjectFactory) override
        {
            if (Instance == nullptr)
            {
                Instance = InNewObjectFactory(Context, *Class.LoadSynchronous());
            }

            return Instance;
        }

        void AddReferencedObjects(FReferenceCollector& Collector) override
        {
            Collector.AddReferencedObject(Instance);
        }

        static FLifetimeHandler* Make(TSoftClassPtr<UObject> InClass) { return new FLifetimeHandler_SingleInstance(MoveTemp(InClass)); }

    private:
        TSoftClassPtr<UObject> Class;
        TObjectPtr<UObject> Instance = nullptr;
    };

    class FLifetimeHandler_WeakSingleInstance : public FLifetimeHandler
    {
    public:
        FLifetimeHandler_WeakSingleInstance(TSoftClassPtr<UObject>&& InClass)
            : Class(MoveTemp(InClass))
        {
        }

        UObject* Get() override { return Instance.Get(); }
        UObject* GetOrCreate(const UObject& Context, FNewObjectFactory InNewObjectFactory) override
        {
            UObject* Result = Instance.Get();
            if (Result == nullptr)
            {
                Instance = Result = InNewObjectFactory(Context, *Class.LoadSynchronous());
            }

            return Result;
        }

        void AddReferencedObjects(FReferenceCollector& Collector) override {}

        static FLifetimeHandler* Make(TSoftClassPtr<UObject> InClass) { return new FLifetimeHandler_WeakSingleInstance(MoveTemp(InClass)); }

    private:
        TSoftClassPtr<UObject> Class;
        TWeakObjectPtr<UObject> Instance = nullptr;
    };
}
