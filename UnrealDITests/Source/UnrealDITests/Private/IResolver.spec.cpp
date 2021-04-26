// Copyright Andrei Sudarikov. All Rights Reserved.

#include "Misc/AutomationTest.h"
#include "Tests/AutomationCommon.h"

#include "DI/ObjectContainerBuilder.h"
#include "DI/ObjectContainer.h"

#include "MockClasses.h"
#include "LatentCommands.h"

BEGIN_DEFINE_SPEC(IResolverSpec, "UnrealDI.IResolver", EAutomationTestFlags::ClientContext | EAutomationTestFlags::EditorContext | EAutomationTestFlags::ServerContext | EAutomationTestFlags::EngineFilter)
END_DEFINE_SPEC(IResolverSpec)

UObjectContainer* BuildContainer()
{
    FObjectContainerBuilder Builder;
    Builder.RegisterType<UMockReader>().As<IReader>().AsSelf();
    return Builder.Build();
}

void IResolverSpec::Define()
{
    It("Should Resolve By UClass", [this]()
    {
        UObject* Reader = BuildContainer()->Resolve(UMockReader::StaticClass());

        TestNotNull("Resolve returned nullptr", Reader);
    });

    It("Should Resolve By Concrete Type Template", [this]()
    {
        UMockReader* Reader = BuildContainer()->Resolve<UMockReader>();

        TestNotNull("Resolve returned nullptr", Reader);
    });

    It("Should Resolve By Interface Template", [this]()
    {
        TScriptInterface<IReader> Reader = BuildContainer()->Resolve<IReader>();

        TestNotNull("Resolve returned nullptr", Reader.GetInterface());
    });
}