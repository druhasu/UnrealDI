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

template <typename T>
T* RegisterAndResolveOptional()
{
    FObjectContainerBuilder Builder;
    Builder.RegisterType<T>();

    UObjectContainer* Container = Builder.Build();

    return Container->Resolve<T>();
}

void DependenciesInjectionSpec::Define()
{
    Describe("Instance", [this]
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

        It("Should Inject optional Interface if registered", [this]()
        {
            auto Resolved = RegisterAndResolve<UNeedOptionalInterfaceInstance>();

            TestNotNull("Resolved object", Resolved);
            TestTrue("Injected dependency is set", Resolved->Instance.IsSet());
            TestNotNull("Injected dependency", Resolved->Instance.GetValue().GetInterface());
        });

        It("Should not Inject optional Interface if not registered", [this]()
        {
            auto Resolved = RegisterAndResolveOptional<UNeedOptionalInterfaceInstance>();

            TestNotNull("Resolved object", Resolved);
            TestFalse("Injected dependency is set", Resolved->Instance.IsSet());
        });
    });

    Describe("Collection", [this]
    {
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

        It("Should Inject optional Interface Collection if registered", [this]()
        {
            auto Resolved = RegisterAndResolve<UNeedOptionalInterfaceCollection>();

            TestNotNull("Resolved object", Resolved);
            TestTrue("Injected dependency is set", Resolved->Collection.IsSet());
            TestTrue("Collection is valid", Resolved->Collection.GetValue().IsValid());
            TestTrue("Collection is not empty", Resolved->Collection.GetValue().Num() > 0);

            for (TScriptInterface<IReader> Interface : Resolved->Collection.GetValue())
            {
                TestNotNull("Object in collection", Interface.GetInterface());
            }
        });

        It("Should not Inject optional Interface Collection if not registered", [this]()
        {
            auto Resolved = RegisterAndResolveOptional<UNeedOptionalInterfaceCollection>();

            TestNotNull("Resolved object", Resolved);
            TestFalse("Injected dependency is set", Resolved->Collection.IsSet());
        });
    });

    Describe("Factory", [this]
    {
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

        It("Should Inject optional Interface Factory if registered", [this]()
        {
            auto Resolved = RegisterAndResolve<UNeedOptionalInterfaceFactory>();

            TestNotNull("Resolved object", Resolved);
            TestTrue("Injected dependency is set", Resolved->Factory.IsSet());
            TestTrue("Factory is valid", Resolved->Factory.GetValue().IsValid());

            auto FromFactory = Resolved->Factory.GetValue()();
            TestNotNull("Factory resolved object", FromFactory.GetInterface());
        });

        It("Should not Inject optional Interface Factory if not registered", [this]()
        {
            auto Resolved = RegisterAndResolveOptional<UNeedOptionalInterfaceFactory>();

            TestNotNull("Resolved object", Resolved);
            TestFalse("Injected dependency is set", Resolved->Factory.IsSet());
        });
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

    It("Should Auto Register type in Container that it was requested from", [this]
    {
        FObjectContainerBuilder ParentBuilder;
        UObjectContainer* ParentContainer = ParentBuilder.Build();

        FObjectContainerBuilder ChildBuilder;
        ChildBuilder.RegisterType<UMockReader>().As<IReader>();
        UObjectContainer* ChildContainer = ChildBuilder.BuildNested(*ParentContainer);

        TestFalse("Registered in ChildContainer", ChildContainer->IsRegistered<UNeedInterfaceInstanceFactory>());

        // this method should search for registration of UNeedInterfaceInstance in ChildContainer
        // it should not find it and auto register UNeedInterfaceInstance in it
        // then it should use this new registration to Inject dependencies into UNeedInterfaceInstanceFactory instance and resolve those dependencies from ChildContainer
        UNeedInterfaceInstanceFactory* Resolved = ChildContainer->Resolve<UNeedInterfaceInstanceFactory>();

        TestNotNull("Resolved object", Resolved);
        TestTrue("Registered in ChildContainer", ChildContainer->IsRegistered<UNeedInterfaceInstanceFactory>());

        TScriptInterface<IReader> Reader = Resolved->Instance->Instance;

        TestNotNull("Reader", Reader.GetInterface());
    });
}
