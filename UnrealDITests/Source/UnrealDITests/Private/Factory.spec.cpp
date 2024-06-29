// Copyright Andrei Sudarikov. All Rights Reserved.

#include "Misc/AutomationTest.h"

#include "DI/Factory.h"
#include "DI/ObjectContainer.h"
#include "DI/ObjectContainerBuilder.h"
#include "BuildContainerHelper.h"
#include "MockReader.h"

BEGIN_DEFINE_SPEC(FFactorySpec, "UnrealDI.Factory", EAutomationTestFlags::ClientContext | EAutomationTestFlags::EditorContext | EAutomationTestFlags::ServerContext | EAutomationTestFlags::EngineFilter)
END_DEFINE_SPEC(FFactorySpec)

void FFactorySpec::Define()
{
    It("Should return UObject", [this]
    {
        UObjectContainer* Container = FBuildContainerHelper::Build();
        TFactory<UMockReader> Factory = Container->ResolveFactory<UMockReader>();

        TestTrue("Factory is Valid", Factory);
        TestTrue("Factory is Valid", Factory.IsValid());

        UMockReader* Resolved = Factory();
        TestNotNull("Resolved object", Resolved);
    });

    It("Should return TScriptInterface", [this]
    {
        UObjectContainer* Container = FBuildContainerHelper::Build();
        TFactory<IReader> Factory = Container->ResolveFactory<IReader>();

        TestTrue("Factory is Valid", Factory);
        TestTrue("Factory is Valid", Factory.IsValid());

        TScriptInterface<IReader> Resolved = Factory();
        TestNotNull("Resolved object", Resolved.GetInterface());
    });

    It("Should be invalid if container destroyed", [this]
    {
        UObjectContainer* Container = FBuildContainerHelper::Build();
        TFactory<IReader> Factory = Container->ResolveFactory<IReader>();

        TestTrue("Factory is Valid", Factory);
        TestTrue("Factory is Valid", Factory.IsValid());

        CollectGarbage(GARBAGE_COLLECTION_KEEPFLAGS);

        TestFalse("Factory is Valid", Factory);
        TestFalse("Factory is Valid", Factory.IsValid());
    });

    It("Should be valid if child container destroyed", [this]
    {
        UObjectContainer* Container = FBuildContainerHelper::Build();
        UObjectContainer* ChildContainer = FObjectContainerBuilder().BuildNested(*Container);

        TFactory<IReader> Factory = ChildContainer->ResolveFactory<IReader>();

        TestTrue("Factory is Valid", Factory);
        TestTrue("Factory is Valid", Factory.IsValid());

        TStrongObjectPtr<UObjectContainer> GCProtector(Container);

        CollectGarbage(GARBAGE_COLLECTION_KEEPFLAGS);

        TestTrue("Factory is Valid", Factory);
        TestTrue("Factory is Valid", Factory.IsValid());
    });
}
