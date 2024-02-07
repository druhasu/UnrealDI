// Copyright Andrei Sudarikov. All Rights Reserved.

#include "Misc/AutomationTest.h"
#include "Tests/AutomationCommon.h"

#include "DI/ObjectContainer.h"
#include "DI/ObjectContainerBuilder.h"
#include "BuildContainerHelper.h"

#include "MockClasses_AutoRegister.h"
//#include "MockReader.h" // Should compile without this header

BEGIN_DEFINE_SPEC(FAutoRegisterSpec, "UnrealDI.Dependencies Injection", EAutomationTestFlags::ClientContext | EAutomationTestFlags::EditorContext | EAutomationTestFlags::ServerContext | EAutomationTestFlags::EngineFilter)
END_DEFINE_SPEC(FAutoRegisterSpec)

void FAutoRegisterSpec::Define()
{
    It("Should Inject Auto Registered Type When Its Dependencies Are Not Included", [this]
    {
        /* This test is in its own file because we need to test that Auto Registration works even if dependencies' headers are not included */
        UObjectContainer* Container = FBuildContainerHelper::Build([](FObjectContainerBuilder& Builder)
        {
            Builder.RegisterType<UWithAutoRegisteredDependency>();
        });

        auto* Resolved = Container->Resolve<UWithAutoRegisteredDependency>();

        TestNotNull("Resolved object", Resolved);
        TestNotNull("Injected Dependency", Resolved->Instance);
    });
}