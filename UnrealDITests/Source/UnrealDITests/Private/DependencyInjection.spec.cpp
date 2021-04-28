// Copyright Andrei Sudarikov. All Rights Reserved.

#include "Misc/AutomationTest.h"
#include "Tests/AutomationCommon.h"

#include "DI/ObjectContainerBuilder.h"
#include "DI/ObjectContainer.h"

#include "MockClasses.h"
#include "LatentCommands.h"

BEGIN_DEFINE_SPEC(DependenciesInjectionSpec, "UnrealDI.Dependencies Injection", EAutomationTestFlags::ClientContext | EAutomationTestFlags::EditorContext | EAutomationTestFlags::ServerContext | EAutomationTestFlags::EngineFilter)
END_DEFINE_SPEC(DependenciesInjectionSpec)

template <typename T>
T* RegisterAndResolve()
{
    FObjectContainerBuilder Builder;
    Builder.RegisterType<UMockReader>().As<IReader>().AsSelf();
    Builder.RegisterType<T>();

    UObjectContainer* Container = Builder.Build();

    return Container->Resolve<T>();
}

void DependenciesInjectionSpec::Define()
{
    It("Should Inject Concrete Type", [this]()
    {
        auto Resolved = RegisterAndResolve<UNeedObjectInstance>();

        TestNotNull("Resolve returned nullptr", Resolved);
        TestNotNull("Dependency not injected", Resolved->Instance);
    });

    It("Should Inject Interface", [this]()
    {
        auto Resolved = RegisterAndResolve<UNeedInterfaceInstance>();

        TestNotNull("Resolve returned nullptr", Resolved);
        TestNotNull("Dependency not injected", Resolved->Instance.GetInterface());
    });

    It("Should Inject Concrete Type Factory", [this]()
    {
        auto Resolved = RegisterAndResolve<UNeedObjectFactory>();

        TestNotNull("Resolve returned nullptr", Resolved);
        TestTrue("Dependency not injected", Resolved->Factory.IsValid());

        auto FromFactory = Resolved->Factory();
        TestNotNull("Factory returned nullptr", FromFactory);
    });

    It("Should Inject Interface Factory", [this]()
    {
        auto Resolved = RegisterAndResolve<UNeedInterfaceFactory>();

        TestNotNull("Resolve returned nullptr", Resolved);
        TestTrue("Dependency not injected", Resolved->Factory.IsValid());

        auto FromFactory = Resolved->Factory();
        TestNotNull("Factory returned nullptr", FromFactory.GetInterface());
    });

    It("Should Inject Concrete Type Collection", [this]()
    {
        auto Resolved = RegisterAndResolve<UNeedObjectCollection>();

        TestNotNull("Resolve returned nullptr", Resolved);
        TestTrue("Collection not valid", Resolved->Collection.IsValid());
        TestTrue("Collection is empty", Resolved->Collection.Num() > 0);

        for (UMockReader* Object : Resolved->Collection)
        {
            TestNotNull("Collection contains nullptr", Object);
        }
    });

    It("Should Inject Interface Collection", [this]()
    {
        auto Resolved = RegisterAndResolve<UNeedInterfaceCollection>();

        TestNotNull("Resolve returned nullptr", Resolved);
        TestTrue("Collection not valid", Resolved->Collection.IsValid());
        TestTrue("Collection is empty", Resolved->Collection.Num() > 0);

        for (TScriptInterface<IReader> Interface : Resolved->Collection)
        {
            TestNotNull("Collection contains nullptr", Interface.GetInterface());
        }
    });

    It("Should Inject Auto Registered Type", [this]()
    {
        FObjectContainerBuilder Builder;
        Builder.RegisterType<UNeedObjectInstance>();

        UObjectContainer* Container = Builder.Build();

        // this call should auto register UMockReader due to dependency from UNeedObjectInstance
        auto Resolved = Container->Resolve<UNeedObjectInstance>();

        TestNotNull("Resolve returned nullptr", Resolved);
        TestNotNull("Dependency not injected", Resolved->Instance);
    });
}