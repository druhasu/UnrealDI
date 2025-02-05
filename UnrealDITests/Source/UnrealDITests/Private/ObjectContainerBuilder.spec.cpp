// Copyright Andrei Sudarikov. All Rights Reserved.

#include "Misc/AutomationTest.h"
#include "Tests/AutomationCommon.h"

#include "DI/ObjectContainerBuilder.h"
#include "DI/ObjectContainer.h"

#include "MockClasses.h"
#include "MockReader.h"
#include "LatentCommands.h"

BEGIN_DEFINE_SPEC(ObjectContainerBuilderSpec, "UnrealDI.ObjectContainerBuilder", EAutomationTestFlags::ClientContext | EAutomationTestFlags::EditorContext | EAutomationTestFlags::ServerContext | EAutomationTestFlags::EngineFilter)
END_DEFINE_SPEC(ObjectContainerBuilderSpec)

void ObjectContainerBuilderSpec::Define()
{
    It("Should Create Empty Container", [this]()
    {
        FObjectContainerBuilder Builder;
        
        UObjectContainer* Container = Builder.Build();
        
        TestNotNull("Container", Container);
    });

    It("Should Create Nested Container", [this]()
    {
        FObjectContainerBuilder Builder;
        UObjectContainer* Container = Builder.Build();

        FObjectContainerBuilder Builder2;
        UObjectContainer* NestedContainer = Builder.BuildNested(*Container);

        TestNotNull("NestedContainer", NestedContainer);
    });

    Describe("Register Type", [this]()
    {
        It("Should Register Type", [this]()
        {
            FObjectContainerBuilder Builder;
            Builder.RegisterType<UMockReader>();

            UObjectContainer* Container = Builder.Build();

            TestNotNull("Container", Container);
            TestTrue("UMockReader is registered", Container->IsRegistered<UMockReader>());
        });

        It("Should Register Type As Self", [this]()
        {
            FObjectContainerBuilder Builder;
            Builder.RegisterType<UMockReader>().AsSelf();

            UObjectContainer* Container = Builder.Build();

            TestNotNull("Container", Container);
            TestTrue("UMockReader is registered", Container->IsRegistered<UMockReader>());
        });

        It("Should Register Type As Interface", [this]()
        {
            FObjectContainerBuilder Builder;
            Builder.RegisterType<UMockReader>().As<IReader>();

            UObjectContainer* Container = Builder.Build();

            TestNotNull("Container", Container);
            TestTrue("IReader is registered", Container->IsRegistered<IReader>());
            TestFalse("UMockReader is registered", Container->IsRegistered<UMockReader>());
        });

        It("Should Register Type As Class", [this]()
        {
            FObjectContainerBuilder Builder;
            Builder.RegisterType<UMockReader>().As<UMockReader>();

            UObjectContainer* Container = Builder.Build();

            TestNotNull("Container", Container);
            TestFalse("IReader is registered", Container->IsRegistered<IReader>());
            TestTrue("UMockReader is registered", Container->IsRegistered<UMockReader>());
        });

        It("Should Register Type As Interface And Self", [this]()
        {
            FObjectContainerBuilder Builder;
            Builder.RegisterType<UMockReader>().As<IReader>().AsSelf();

            UObjectContainer* Container = Builder.Build();

            TestNotNull("Container", Container);
            TestTrue("UMockReader is registered", Container->IsRegistered<UMockReader>());
            TestTrue("IReader is registered", Container->IsRegistered<IReader>());
        });

        It("Should Register Type By Interfaces", [this]()
        {
            FObjectContainerBuilder Builder;
            Builder.RegisterType<UMockReader>().ByInterfaces();

            UObjectContainer* Container = Builder.Build();

            TestNotNull("Container", Container);
            TestTrue("IReader is registered", Container->IsRegistered<IReader>());
            TestFalse("UMockReader is registered", Container->IsRegistered<UMockReader>());
        });

        It("Should Register Type By Interfaces And Self", [this]()
        {
            FObjectContainerBuilder Builder;
            Builder.RegisterType<UMockReader>().ByInterfaces().AsSelf();

            UObjectContainer* Container = Builder.Build();

            TestNotNull("Container", Container);
            TestTrue("UMockReader is registered", Container->IsRegistered<UMockReader>());
            TestTrue("IReader is registered", Container->IsRegistered<IReader>());
        });

        It("Should Register Type From Blueprint By Class", [this]
        {
            TSoftClassPtr<UMockReader> SoftClassPtr(FSoftObjectPath(TEXT("/UnrealDITests/BP_MockReader.BP_MockReader_C")));
            TSubclassOf<UMockReader> ClassPtr = SoftClassPtr.LoadSynchronous();

            FObjectContainerBuilder Builder;
            Builder.RegisterType<UMockReader>().FromBlueprint(ClassPtr);

            UObjectContainer* Container = Builder.Build();

            TestNotNull("Container", Container);
            TestTrue("UMockReader is registered", Container->IsRegistered<UMockReader>());

            UMockReader* Reader = Container->Resolve<UMockReader>();
            TestNotNull("Blueprint Class", Cast<UBlueprintGeneratedClass>(Reader->GetClass()));
        });

        It("Should Register Type From Blueprint By Path", [this]
        {
            TSoftClassPtr<UMockReader> SoftClassPtr(FSoftObjectPath(TEXT("/UnrealDITests/BP_MockReader.BP_MockReader_C")));

            FObjectContainerBuilder Builder;
            Builder.RegisterType<UMockReader>().FromBlueprint(SoftClassPtr);

            UObjectContainer* Container = Builder.Build();

            TestNotNull("Container", Container);
            TestTrue("UMockReader is registered", Container->IsRegistered<UMockReader>());

            UMockReader* Reader = Container->Resolve<UMockReader>();
            TestNotNull("Blueprint Class", Cast<UBlueprintGeneratedClass>(Reader->GetClass()));
        });
    });

    Describe("Register Instance", [this]()
    {
        It("Should Register Instance", [this]()
        {
            FObjectContainerBuilder Builder;
            Builder.RegisterInstance(NewObject<UMockReader>());

            UObjectContainer* Container = Builder.Build();

            TestNotNull("Container", Container);
            TestTrue("UMockReader is registered", Container->IsRegistered<UMockReader>());
        });

        It("Should Register Instance As Self", [this]()
        {
            FObjectContainerBuilder Builder;
            Builder.RegisterInstance(NewObject<UMockReader>()).AsSelf();

            UObjectContainer* Container = Builder.Build();

            TestNotNull("Container", Container);
            TestTrue("UMockReader is registered", Container->IsRegistered<UMockReader>());
        });

        It("Should Register Instance As Interface", [this]()
        {
            FObjectContainerBuilder Builder;
            Builder.RegisterInstance(NewObject<UMockReader>()).As<IReader>();

            UObjectContainer* Container = Builder.Build();

            TestNotNull("Container", Container);
            TestTrue("IReader is registered", Container->IsRegistered<IReader>());
            TestFalse("UMockReader is registered", Container->IsRegistered<UMockReader>());
        });

        It("Should Register Instance As Class", [this]()
        {
            FObjectContainerBuilder Builder;
            Builder.RegisterInstance(NewObject<UMockReader>()).As<UMockReader>();

            UObjectContainer* Container = Builder.Build();

            TestNotNull("Container", Container);
            TestFalse("IReader is registered", Container->IsRegistered<IReader>());
            TestTrue("UMockReader is registered", Container->IsRegistered<UMockReader>());
        });

        It("Should Register Instance As Interface And Self", [this]()
        {
            FObjectContainerBuilder Builder;
            Builder.RegisterInstance(NewObject<UMockReader>()).As<IReader>().AsSelf();

            UObjectContainer* Container = Builder.Build();

            TestNotNull("Container", Container);
            TestTrue("IReader is registered", Container->IsRegistered<IReader>());
            TestTrue("UMockReader is registered", Container->IsRegistered<UMockReader>());
        });

        It("Should Register Instance By Interfaces", [this]()
        {
            FObjectContainerBuilder Builder;
            Builder.RegisterInstance(NewObject<UMockReader>()).ByInterfaces();

            UObjectContainer* Container = Builder.Build();

            TestNotNull("Container", Container);
            TestTrue("IReader is registered", Container->IsRegistered<IReader>());
            TestFalse("UMockReader is registered", Container->IsRegistered<UMockReader>());
        });

        It("Should Register Instance By Interfaces And Self", [this]()
        {
            FObjectContainerBuilder Builder;
            Builder.RegisterInstance(NewObject<UMockReader>()).ByInterfaces().AsSelf();

            UObjectContainer* Container = Builder.Build();

            TestNotNull("Container", Container);
            TestTrue("IReader is registered", Container->IsRegistered<IReader>());
            TestTrue("UMockReader is registered", Container->IsRegistered<UMockReader>());
        });
    });

    Describe("Register Factory", [this]()
    {
        It("Should Register Factory", [this]()
        {
            FObjectContainerBuilder Builder;
            Builder.RegisterFactory<UMockReader>([]() { return NewObject<UMockReader>(); });

            UObjectContainer* Container = Builder.Build();

            TestNotNull("Container", Container);
            TestTrue("UMockReader is registered", Container->IsRegistered<UMockReader>());
        });

        It("Should Register Factory As Self", [this]()
        {
            FObjectContainerBuilder Builder;
            Builder.RegisterFactory<UMockReader>([]() { return NewObject<UMockReader>(); }).AsSelf();

            UObjectContainer* Container = Builder.Build();

            TestNotNull("Container", Container);
            TestTrue("UMockReader is registered", Container->IsRegistered<UMockReader>());
        });

        It("Should Register Factory As Interface", [this]()
        {
            FObjectContainerBuilder Builder;
            Builder.RegisterFactory<UMockReader>([]() { return NewObject<UMockReader>(); }).As<IReader>();

            UObjectContainer* Container = Builder.Build();

            TestNotNull("Container", Container);
            TestTrue("IReader is registered", Container->IsRegistered<IReader>());
            TestFalse("UMockReader is registered", Container->IsRegistered<UMockReader>());
        });

        It("Should Register Factory As Class", [this]()
        {
            FObjectContainerBuilder Builder;
            Builder.RegisterFactory<UMockReader>([]() { return NewObject<UMockReader>(); }).As<UMockReader>();

            UObjectContainer* Container = Builder.Build();

            TestNotNull("Container", Container);
            TestFalse("IReader is registered", Container->IsRegistered<IReader>());
            TestTrue("UMockReader is registered", Container->IsRegistered<UMockReader>());
        });

        It("Should Register Factory As Interface And Self", [this]()
        {
            FObjectContainerBuilder Builder;
            Builder.RegisterFactory<UMockReader>([]() { return NewObject<UMockReader>(); }).As<IReader>().AsSelf();

            UObjectContainer* Container = Builder.Build();

            TestNotNull("Container", Container);
            TestTrue("IReader is registered", Container->IsRegistered<IReader>());
            TestTrue("UMockReader is registered", Container->IsRegistered<UMockReader>());
        });

        It("Should Register Factory By Interfaces", [this]()
        {
            FObjectContainerBuilder Builder;
            Builder.RegisterFactory<UMockReader>([]() { return NewObject<UMockReader>(); }).ByInterfaces();

            UObjectContainer* Container = Builder.Build();

            TestNotNull("Container", Container);
            TestTrue("IReader is registered", Container->IsRegistered<IReader>());
            TestFalse("UMockReader is registered", Container->IsRegistered<UMockReader>());
        });

        It("Should Register Factory By Interfaces And Self", [this]()
        {
            FObjectContainerBuilder Builder;
            Builder.RegisterFactory<UMockReader>([]() { return NewObject<UMockReader>(); }).ByInterfaces().AsSelf();

            UObjectContainer* Container = Builder.Build();

            TestNotNull("Container", Container);
            TestTrue("IReader is registered", Container->IsRegistered<IReader>());
            TestTrue("UMockReader is registered", Container->IsRegistered<UMockReader>());
        });
    });

    Describe("Register Default", [this]()
    {
        It("Should Register Default", [this]()
        {
            FObjectContainerBuilder Builder;
            Builder.RegisterDefault<UMockReader>();

            UObjectContainer* Container = Builder.Build();

            TestNotNull("Container", Container);
            TestTrue("UMockReader is registered", Container->IsRegistered<UMockReader>());
        });

        It("Should Register Default As Self", [this]()
        {
            FObjectContainerBuilder Builder;
            Builder.RegisterDefault<UMockReader>().AsSelf();

            UObjectContainer* Container = Builder.Build();

            TestNotNull("Container", Container);
            TestTrue("UMockReader is registered", Container->IsRegistered<UMockReader>());
        });

        It("Should Register Default As Interface", [this]()
        {
            FObjectContainerBuilder Builder;
            Builder.RegisterDefault<UMockReader>().As<IReader>();

            UObjectContainer* Container = Builder.Build();

            TestNotNull("Container", Container);
            TestTrue("IReader is registered", Container->IsRegistered<IReader>());
            TestFalse("UMockReader is registered", Container->IsRegistered<UMockReader>());
        });

        It("Should Register Default As Class", [this]()
        {
            FObjectContainerBuilder Builder;
            Builder.RegisterDefault<UMockReader>().As<UMockReader>();

            UObjectContainer* Container = Builder.Build();

            TestNotNull("Container", Container);
            TestFalse("IReader is registered", Container->IsRegistered<IReader>());
            TestTrue("UMockReader is registered", Container->IsRegistered<UMockReader>());
        });

        It("Should Register Default As Interface And Self", [this]()
        {
            FObjectContainerBuilder Builder;
            Builder.RegisterDefault<UMockReader>().As<IReader>().AsSelf();

            UObjectContainer* Container = Builder.Build();

            TestNotNull("Container", Container);
            TestTrue("IReader is registered", Container->IsRegistered<IReader>());
            TestTrue("UMockReader is registered", Container->IsRegistered<UMockReader>());
        });

        It("Should Register Default By Interfaces", [this]()
        {
            FObjectContainerBuilder Builder;
            Builder.RegisterDefault<UMockReader>().ByInterfaces();

            UObjectContainer* Container = Builder.Build();

            TestNotNull("Container", Container);
            TestTrue("IReader is registered", Container->IsRegistered<IReader>());
            TestFalse("UMockReader is registered", Container->IsRegistered<UMockReader>());
        });

        It("Should Register Default By Interfaces And Self", [this]()
        {
            FObjectContainerBuilder Builder;
            Builder.RegisterDefault<UMockReader>().ByInterfaces().AsSelf();

            UObjectContainer* Container = Builder.Build();

            TestNotNull("Container", Container);
            TestTrue("IReader is registered", Container->IsRegistered<IReader>());
            TestTrue("UMockReader is registered", Container->IsRegistered<UMockReader>());
        });

        It("Should Resolve Default Object", [this]()
        {
            FObjectContainerBuilder Builder;
            Builder.RegisterDefault<UMockReader>();

            UObjectContainer* Container = Builder.Build();
            UMockReader* Resolved = Container->Resolve<UMockReader>();

            TestEqual("Resolved object", Resolved, GetMutableDefault<UMockReader>());
        });
    });

    Describe("Outer For New Objects", [this]()
    {
        It("Should create Objects with same Outer as regular Container if not overriden", [this]
        {
            FObjectContainerBuilder Builder;
            Builder.RegisterType<UMockReader>();

            UObjectContainer* Container = Builder.Build();

            UMockReader* Reader = Container->Resolve<UMockReader>();

            TestEqual("Outer", Reader->GetOuter(), Container->GetOuter());
        });

        It("Should create Objects with same Outer as parent Container of nested Container if not overriden", [this]
        {
            FObjectContainerBuilder Builder;

            FObjectContainerBuilder NestedBuilder;
            NestedBuilder.RegisterType<UMockReader>();

            UObjectContainer* Container = Builder.Build();
            UObjectContainer* NestedContainer = NestedBuilder.BuildNested(*Container);

            UMockReader* Reader = NestedContainer->Resolve<UMockReader>();

            TestEqual("Outer", Reader->GetOuter(), Container->GetOuter());
        });

        It("Should create objects with custom Outer if overriden in regular Container", [this]
        {
            UObject* A = NewObject<UTestOuter>();

            FObjectContainerBuilder Builder;
            Builder.RegisterType<UMockReader>();
            Builder.SetOuterForNewObjects(A);

            UObjectContainer* Container = Builder.Build();

            UMockReader* Reader = Container->Resolve<UMockReader>();

            TestEqual("Outer", Reader->GetOuter(), A);
        });

        It("Should create objects with custom Outer if overriden in parent Container", [this]
        {
            UObject* A = NewObject<UTestOuter>();

            FObjectContainerBuilder Builder;
            Builder.SetOuterForNewObjects(A);

            FObjectContainerBuilder NestedBuilder;
            NestedBuilder.RegisterType<UMockReader>();

            UObjectContainer* Container = Builder.Build();
            UObjectContainer* NestedContainer = NestedBuilder.BuildNested(*Container);

            UMockReader* Reader = NestedContainer->Resolve<UMockReader>();

            TestEqual("Outer", Reader->GetOuter(), A);
        });

        It("Should create objects with custom Outer if overriden in nested Container", [this]
        {
            UObject* A = NewObject<UTestOuter>();

            FObjectContainerBuilder Builder;

            FObjectContainerBuilder NestedBuilder;
            NestedBuilder.RegisterType<UMockReader>();
            NestedBuilder.SetOuterForNewObjects(A);

            UObjectContainer* Container = Builder.Build();
            UObjectContainer* NestedContainer = NestedBuilder.BuildNested(*Container);

            UMockReader* Reader = NestedContainer->Resolve<UMockReader>();

            TestEqual("Outer", Reader->GetOuter(), A);
        });

        It("Should use Outer from nested Container if registered in nested and resolved from nested", [this]
        {
            UObject* A = NewObject<UTestOuter>();
            UObject* B = NewObject<UTestOuter>();

            FObjectContainerBuilder Builder;
            Builder.SetOuterForNewObjects(A);

            FObjectContainerBuilder NestedBuilder;
            NestedBuilder.SetOuterForNewObjects(B);
            NestedBuilder.RegisterType<UMockReader>();

            UObjectContainer* Container = Builder.Build();
            UObjectContainer* NestedContainer = NestedBuilder.BuildNested(*Container);

            UMockReader* Reader = NestedContainer->Resolve<UMockReader>();

            TestEqual("Outer", Reader->GetOuter(), B);
        });

        It("Should use Outer from parent Container if registered in parent and resolved from nested", [this]
        {
            UObject* A = NewObject<UTestOuter>();
            UObject* B = NewObject<UTestOuter>();

            FObjectContainerBuilder Builder;
            Builder.SetOuterForNewObjects(A);
            Builder.RegisterType<UMockReader>();

            FObjectContainerBuilder NestedBuilder;
            NestedBuilder.SetOuterForNewObjects(B);

            UObjectContainer* Container = Builder.Build();
            UObjectContainer* NestedContainer = NestedBuilder.BuildNested(*Container);

            UMockReader* Reader = NestedContainer->Resolve<UMockReader>();

            TestEqual("Outer", Reader->GetOuter(), A);
        });

        It("Should use Outer from respective container when resolving collection", [this]
        {
            UObject* A = NewObject<UTestOuter>();
            UObject* B = NewObject<UTestOuter>();

            FObjectContainerBuilder Builder;
            Builder.SetOuterForNewObjects(A);
            Builder.RegisterType<UMockReader>();

            FObjectContainerBuilder NestedBuilder;
            NestedBuilder.SetOuterForNewObjects(B);
            NestedBuilder.RegisterType<UMockReader>();

            UObjectContainer* Container = Builder.Build();
            UObjectContainer* NestedContainer = NestedBuilder.BuildNested(*Container);

            TArray<UMockReader*> Readers = NestedContainer->ResolveAll<UMockReader>().ToArray();

            TestEqual("First Reader Outer", Readers[0]->GetOuter(), A);
            TestEqual("Second Reader Outer", Readers[1]->GetOuter(), B);
        });
    });
}
