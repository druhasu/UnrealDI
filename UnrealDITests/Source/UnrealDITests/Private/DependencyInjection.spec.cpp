// Copyright Andrei Sudarikov. All Rights Reserved.

#include "Misc/AutomationTest.h"
#include "Tests/AutomationCommon.h"

#include "DI/ObjectContainerBuilder.h"
#include "DI/ObjectContainer.h"

#include "MockClasses.h"
#include "MockReader.h"
#include "BuildContainerHelper.h"

BEGIN_DEFINE_SPEC(DependenciesInjectionSpec, "UnrealDI.Dependencies Injection", EAutomationTestFlags::ClientContext | EAutomationTestFlags::EditorContext | EAutomationTestFlags::ServerContext | EAutomationTestFlags::EngineFilter)
END_DEFINE_SPEC(DependenciesInjectionSpec)

template <typename T>
T* RegisterAndResolve()
{
    UObjectContainer* Container = FBuildContainerHelper::Build([](FObjectContainerBuilder& Builder)
    {
        Builder.RegisterType<T>();
    });

    return Container->Resolve<T>();
}

void DependenciesInjectionSpec::Define()
{
    It("Should Inject Concrete Type", [this]()
    {
        auto Resolved = RegisterAndResolve<UNeedObjectInstance>();

        TestNotNull("Resolved object", Resolved);
        TestNotNull("Injected dependency", Resolved->Instance);
    });

    It("Should Inject Concrete Type via TObjectPtr", [this]()
    {
        auto Resolved = RegisterAndResolve<UNeedObjectPtrInstance>();

        TestNotNull("Resolved object", Resolved);
        TestNotNull("Injected dependency", Resolved->Instance.Get());
    });

    It("Should Inject Interface", [this]()
    {
        auto Resolved = RegisterAndResolve<UNeedInterfaceInstance>();

        TestNotNull("Resolved object", Resolved);
        TestNotNull("Injected dependency", Resolved->Instance.GetInterface());
    });

    It("Should Inject Concrete Type Factory", [this]()
    {
        auto Resolved = RegisterAndResolve<UNeedObjectFactory>();

        TestNotNull("Resolved object", Resolved);
        TestTrue("Factory is valid", Resolved->Factory.IsValid());

        auto FromFactory = Resolved->Factory();
        TestNotNull("Factory resolved object", FromFactory);
    });

    It("Should Inject Interface Factory", [this]()
    {
        auto Resolved = RegisterAndResolve<UNeedInterfaceFactory>();

        TestNotNull("Resolved object", Resolved);
        TestTrue("Factory is valid", Resolved->Factory.IsValid());

        auto FromFactory = Resolved->Factory();
        TestNotNull("Factory resolved object", FromFactory.GetInterface());
    });

    It("Should Inject Concrete Type Collection", [this]()
    {
        auto Resolved = RegisterAndResolve<UNeedObjectCollection>();

        TestNotNull("Resolved object", Resolved);
        TestTrue("Collection is valid", Resolved->Collection.IsValid());
        TestTrue("Collection is not empty", Resolved->Collection.Num() > 0);

        for (UMockReader* Object : Resolved->Collection)
        {
            TestNotNull("Object in collection", Object);
        }
    });

    It("Should Inject Interface Collection", [this]()
    {
        auto Resolved = RegisterAndResolve<UNeedInterfaceCollection>();

        TestNotNull("Resolved object", Resolved);
        TestTrue("Collection is valid", Resolved->Collection.IsValid());
        TestTrue("Collection is not empty", Resolved->Collection.Num() > 0);

        for (TScriptInterface<IReader> Interface : Resolved->Collection)
        {
            TestNotNull("Object in collection", Interface.GetInterface());
        }
    });

    It("Should Inject Custom dependency", [this]
    {
        auto Resolved = RegisterAndResolve<UNeedTestDependency>();

        TestNotNull("Resolved object", Resolved);
        TestNotNull("Injected dependency", Resolved->Reader);
    });

    It("Should Inject Auto Registered Type", [this]()
    {
        FObjectContainerBuilder Builder;
        Builder.RegisterType<UNeedObjectInstance>();

        UObjectContainer* Container = Builder.Build();

        // this call should auto register UMockReader due to dependency from UNeedObjectInstance
        auto Resolved = Container->Resolve<UNeedObjectInstance>();

        TestNotNull("Resolved object", Resolved);
        TestNotNull("Injected dependency", Resolved->Instance);
    });
}
