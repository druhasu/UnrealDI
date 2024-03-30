// Copyright Andrei Sudarikov. All Rights Reserved.

#include "Misc/AutomationTest.h"
#include "Runtime/Launch/Resources/Version.h"
#include "Tests/AutomationCommon.h"

#include "DI/ObjectContainerBuilder.h"
#include "DI/ObjectContainer.h"

#include "MockReader.h"
#include "BuildContainerHelper.h"

BEGIN_DEFINE_SPEC(FInvokeWithDependenciesSpec, "UnrealDI.Invoke With Dependencies", EAutomationTestFlags::ClientContext | EAutomationTestFlags::EditorContext | EAutomationTestFlags::ServerContext | EAutomationTestFlags::EngineFilter)
END_DEFINE_SPEC(FInvokeWithDependenciesSpec)

void FInvokeWithDependenciesSpec::Define()
{
    It("Should invoke with Concrete Type", [this]()
    {
        FBuildContainerHelper::Build()->InvokeWithDependencies([this](UMockReader* Instance)
        {
            TestNotNull("Injected Object", Instance);
        });
    });

    It("Should invoke with Concrete Type via TObjectPtr", [this]()
    {
        FBuildContainerHelper::Build()->InvokeWithDependencies([this](TObjectPtr<UMockReader> Instance)
        {
            TestNotNull("Injected ObjectPtr", Instance.Get());
        });
    });

    It("Should invoke with Interface", [this]()
    {
        FBuildContainerHelper::Build()->InvokeWithDependencies([this](TScriptInterface<IReader>&& Interface)
        {
            TestNotNull("Injected Interface", Interface.GetInterface());
        });
    });

    It("Should invoke with Concrete Type Factory", [this]()
    {
        FBuildContainerHelper::Build()->InvokeWithDependencies([this](TFactory<UMockReader>&& Factory)
        {
            TestTrue("Injected Factory valid", Factory.IsValid());

            auto FromFactory = Factory();
            TestNotNull("Factory created object", FromFactory);
        });
    });

    It("Should invoke with Interface Factory", [this]()
    {
        FBuildContainerHelper::Build()->InvokeWithDependencies([this](TFactory<IReader>&& Factory)
        {
            TestTrue("Injected Factory valid", Factory.IsValid());

            auto FromFactory = Factory();
            TestNotNull("Factory created interface", FromFactory.GetInterface());
        });
    });

    It("Should invoke with Concrete Type Collection", [this]()
    {
        FBuildContainerHelper::Build()->InvokeWithDependencies([this](TObjectsCollection<UMockReader>&& Collection)
        {
            TestTrue("Collection is valid", Collection.IsValid());
            TestTrue("Collection is not empty", Collection.Num() > 0);

            for (UMockReader* Object : Collection)
            {
                TestNotNull("Collection item", Object);
            }
        });
    });

    It("Should invoke with Interface Collection", [this]()
    {
        FBuildContainerHelper::Build()->InvokeWithDependencies([this](TObjectsCollection<IReader>&& Collection)
        {
            TestTrue("Collection is valid", Collection.IsValid());
            TestTrue("Collection is not empty", Collection.Num() > 0);

            for (TScriptInterface<IReader> Interface : Collection)
            {
                TestNotNull("Collection item", Interface.GetInterface());
            }
        });
    });

    It("Should invoke with multiple types", [this]()
    {
        FBuildContainerHelper::Build()->InvokeWithDependencies([this](UMockReader* Instance, TScriptInterface<IReader>&& Interface, TFactory<UMockReader>&& Factory, TObjectsCollection<IReader>&& Collection)
        {
            TestNotNull("Injected Object", Instance);
            TestNotNull("Injected Interface", Interface.GetInterface());

            TestTrue("Injected Factory valid", Factory.IsValid());
            auto FromFactory = Factory();
            TestNotNull("Factory created object", FromFactory);

            TestTrue("Collection is valid", Collection.IsValid());
            TestTrue("Collection is not empty", Collection.Num() > 0);

            for (TScriptInterface<IReader> Reader : Collection)
            {
                TestNotNull("Collection item", Reader.GetInterface());
            }
        });
    });
}
