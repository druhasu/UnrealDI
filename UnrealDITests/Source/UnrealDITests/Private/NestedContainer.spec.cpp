// Copyright Andrei Sudarikov. All Rights Reserved.

#include "Misc/AutomationTest.h"
#include "Tests/AutomationCommon.h"

#include "DI/ObjectContainerBuilder.h"
#include "DI/ObjectContainer.h"

#include "MockClasses.h"
#include "LatentCommands.h"

BEGIN_DEFINE_SPEC(NestedContainerSpec, "UnrealDI.NestedContainer", EAutomationTestFlags::ClientContext | EAutomationTestFlags::EditorContext | EAutomationTestFlags::ServerContext | EAutomationTestFlags::EngineFilter)
END_DEFINE_SPEC(NestedContainerSpec)

void NestedContainerSpec::Define()
{
    Describe("Resolve", [this]()
    {
        It("Should Resolve From Parent When Absent in Nested", [this]()
        {
            UMockReader* ParentReader = NewObject<UMockReader>();

            FObjectContainerBuilder ParentBuilder;
            ParentBuilder.RegisterInstance(ParentReader);
            UObjectContainer* ParentContainer = ParentBuilder.Build();

            FObjectContainerBuilder NestedBuilder;
            UObjectContainer* NestedContainer = NestedBuilder.BuildNested(*ParentContainer);

            UMockReader* Resolved = NestedContainer->Resolve<UMockReader>();

            TestNotNull("Resolved nullptr", Resolved);
            TestEqual("Resolved wrong object", Resolved, ParentReader);
        });

        It("Should Resolve From Nested When Absent in Parent", [this]()
        {
            UMockReader* NestedReader = NewObject<UMockReader>();

            FObjectContainerBuilder ParentBuilder;
            UObjectContainer* ParentContainer = ParentBuilder.Build();

            FObjectContainerBuilder NestedBuilder;
            NestedBuilder.RegisterInstance(NestedReader);
            UObjectContainer* NestedContainer = NestedBuilder.BuildNested(*ParentContainer);

            UMockReader* Resolved = NestedContainer->Resolve<UMockReader>();

            TestNotNull("Resolved nullptr", Resolved);
            TestEqual("Resolved wrong object", Resolved, NestedReader);
        });

        It("Should Resolve From Nested When Present In Both", [this]()
        {
            UMockReader* ParentReader = NewObject<UMockReader>();
            UMockReader* NestedReader = NewObject<UMockReader>();

            FObjectContainerBuilder ParentBuilder;
            ParentBuilder.RegisterInstance(ParentReader);
            UObjectContainer* ParentContainer = ParentBuilder.Build();

            FObjectContainerBuilder NestedBuilder;
            NestedBuilder.RegisterInstance(NestedReader);
            UObjectContainer* NestedContainer = NestedBuilder.BuildNested(*ParentContainer);

            UMockReader* Resolved = NestedContainer->Resolve<UMockReader>();

            TestNotNull("Resolved nullptr", Resolved);
            TestEqual("Resolved wrong object", Resolved, NestedReader);
        });
    });

    Describe("ResolveAll", [this]()
    {
        It("Should ResolveAll From Parent When Absent In Nested", [this]()
        {
            UMockReader* ParentReader = NewObject<UMockReader>();

            FObjectContainerBuilder ParentBuilder;
            ParentBuilder.RegisterInstance(ParentReader);
            UObjectContainer* ParentContainer = ParentBuilder.Build();

            FObjectContainerBuilder NestedBuilder;
            UObjectContainer* NestedContainer = NestedBuilder.BuildNested(*ParentContainer);

            TObjectsCollection<UMockReader> Resolved = NestedContainer->ResolveAll<UMockReader>();
            TArray<UMockReader*> ResolvedArray = Resolved.ToArray();

            TestTrue("Resolved invalid collection", Resolved.IsValid());
            TestTrue("Resolved incorrect amount of objects", Resolved.Num() == 1);
            TestEqual("Resolved[0] contains wrong object", ResolvedArray[0], ParentReader);
        });

        It("Should ResolveAll From Nested When Absent In Parent", [this]()
        {
            UMockReader* NestedReader = NewObject<UMockReader>();

            FObjectContainerBuilder ParentBuilder;
            UObjectContainer* ParentContainer = ParentBuilder.Build();

            FObjectContainerBuilder NestedBuilder;
            NestedBuilder.RegisterInstance(NestedReader);
            UObjectContainer* NestedContainer = NestedBuilder.BuildNested(*ParentContainer);

            TObjectsCollection<UMockReader> Resolved = NestedContainer->ResolveAll<UMockReader>();
            TArray<UMockReader*> ResolvedArray = Resolved.ToArray();

            TestTrue("Resolved invalid collection", Resolved.IsValid());
            TestTrue("Resolved incorrect amount of objects", Resolved.Num() == 1);
            TestEqual("Resolved[0] contains wrong object", ResolvedArray[0], NestedReader);
        });

        It("Should ResolveAll From Nested And Parent When Present In Both", [this]()
        {
            UMockReader* ParentReader = NewObject<UMockReader>();
            UMockReader* NestedReader = NewObject<UMockReader>();

            FObjectContainerBuilder ParentBuilder;
            ParentBuilder.RegisterInstance(ParentReader);
            UObjectContainer* ParentContainer = ParentBuilder.Build();

            FObjectContainerBuilder NestedBuilder;
            NestedBuilder.RegisterInstance(NestedReader);
            UObjectContainer* NestedContainer = NestedBuilder.BuildNested(*ParentContainer);

            TObjectsCollection<UMockReader> Resolved = NestedContainer->ResolveAll<UMockReader>();
            TArray<UMockReader*> ResolvedArray = Resolved.ToArray();

            TestTrue("Resolved invalid collection", Resolved.IsValid());
            TestTrue("Resolved incorrect amount of objects", Resolved.Num() == 2);
            TestEqual("Resolved[0] contains wrong object", ResolvedArray[0], ParentReader);
            TestEqual("Resolved[1] contains wrong object", ResolvedArray[1], NestedReader);
        });
    });
}