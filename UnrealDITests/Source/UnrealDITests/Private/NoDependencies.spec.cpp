// Copyright Andrei Sudarikov. All Rights Reserved.

#include "Misc/AutomationTest.h"
#include "Tests/AutomationCommon.h"

#include "DI/ObjectContainerBuilder.h"
#include "DI/ObjectContainer.h"

#include "MockClasses_EmptyDependencies.h"
#include "LatentCommands.h"
#include "TempWorldHelper.h"

BEGIN_DEFINE_SPEC(NoDependenciesSpec, "UnrealDI.No Dependencies Init", EAutomationTestFlags::ClientContext | EAutomationTestFlags::EditorContext | EAutomationTestFlags::ServerContext | EAutomationTestFlags::EngineFilter)
END_DEFINE_SPEC(NoDependenciesSpec)

void NoDependenciesSpec::Define()
{
    It("Should Call Empty InitDependencies on UObject", [this]()
    {
        FTempWorldHelper Helper;

        FObjectContainerBuilder Builder;
        Builder.RegisterType<UObjectNoDeps>();
        UObjectContainer* Container = Builder.Build(Helper.World);

        auto Obj = Container->Resolve<UObjectNoDeps>();

        TestTrue("InitDependencies called", Obj->bWasCalled);
    });

    It("Should Call Empty InitDependencies on Actor", [this]()
    {
        FTempWorldHelper Helper;

        FObjectContainerBuilder Builder;
        Builder.RegisterType<AActorNoDeps>();
        UObjectContainer* Container = Builder.Build(Helper.World);

        auto Obj = Container->Resolve<AActorNoDeps>();

        TestTrue("InitDependencies called", Obj->bWasCalled);
    });

    It("Should Call Empty InitDependencies on Widget", [this]()
    {
        FTempWorldHelper Helper;

        FObjectContainerBuilder Builder;
        Builder.RegisterType<UWidgetNoDeps>();
        UObjectContainer* Container = Builder.Build(Helper.World);

        auto Obj = Container->Resolve<UWidgetNoDeps>();

        TestTrue("InitDependencies called", Obj->bWasCalled);
    });
}