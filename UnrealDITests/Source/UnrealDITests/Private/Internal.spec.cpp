// Copyright Andrei Sudarikov. All Rights Reserved.

#include "Misc/AutomationTest.h"

#include "MockClasses.h"
#include "LatentCommands.h"

/*
 * Spec for testing helper classes of this module, not UnrealDI itself
 */
BEGIN_DEFINE_SPEC(UnrealDIInternalSpec, "UnrealDI.Internal", EAutomationTestFlags::ClientContext | EAutomationTestFlags::EditorContext | EAutomationTestFlags::ServerContext | EAutomationTestFlags::EngineFilter)
END_DEFINE_SPEC(UnrealDIInternalSpec)

void UnrealDIInternalSpec::Define()
{
    Describe("Garbage Collection", [this]()
    {
        It("Should Collect Garbage", [this]()
        {
            TWeakObjectPtr<UMockReader> ReaderPtr = NewObject<UMockReader>();

            ADD_LATENT_AUTOMATION_COMMAND(FRunGC);
            ADD_LATENT_AUTOMATION_COMMAND(FFunctionLatentCommand([this, ReaderPtr]()
            {
                TestFalse("Object not destroyed", ReaderPtr.IsValid());
                return true;
            }));
        });

        It("Should Not Collect Garbage", [this]()
        {
            TWeakObjectPtr<UMockReader> ReaderPtr = NewObject<UMockReader>();
            ReaderPtr->AddToRoot();

            ADD_LATENT_AUTOMATION_COMMAND(FRunGC);
            ADD_LATENT_AUTOMATION_COMMAND(FFunctionLatentCommand([this, ReaderPtr]()
            {
                TestTrue("Object was destroyed", ReaderPtr.IsValid());
                ReaderPtr->RemoveFromRoot();
                return true;
            }));
        });
    });
}