// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "UObject/Object.h"

namespace UnrealDI_Impl
{
    class FRegistrationStorage;

    class FLifetimeHandler
    {
    public:
        virtual ~FLifetimeHandler() = default;

        virtual UObject* Get() = 0;
        virtual void Set(UObject* Object) = 0;
        virtual void AddReferencedObjects(FReferenceCollector& Collector) = 0;
    };

    class FLifetimeHandler_Transient : public FLifetimeHandler
    {
    public:
        UObject* Get() override { return nullptr; }
        void Set(UObject* Object) override {}
        void AddReferencedObjects(FReferenceCollector& Collector) override {}

        static TSharedRef<FLifetimeHandler> Make() { return MakeShared<FLifetimeHandler_Transient>(); }
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
        void Set(UObject* Object) override {}
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
        void Set(UObject* Object) override {}
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
        void Set(UObject* Object) override {}
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
        UObject* Get() override { return Instance; }
        void Set(UObject* Object) override { Instance = Object; }
        void AddReferencedObjects(FReferenceCollector& Collector) override
        {
            Collector.AddReferencedObject(Instance);
        }

        static TSharedRef<FLifetimeHandler> Make() { return MakeShared<FLifetimeHandler_SingleInstance>(); }

    private:
        UObject* Instance = nullptr;
    };

    class FLifetimeHandler_WeakSingleInstance : public FLifetimeHandler
    {
    public:
        UObject* Get() override { return Instance.Get(); }
        void Set(UObject* Object) override { Instance = Object; }
        void AddReferencedObjects(FReferenceCollector& Collector) override {}

        static TSharedRef<FLifetimeHandler> Make() { return MakeShared<FLifetimeHandler_WeakSingleInstance>(); }

    private:
        TWeakObjectPtr<UObject> Instance = nullptr;
    };
}
