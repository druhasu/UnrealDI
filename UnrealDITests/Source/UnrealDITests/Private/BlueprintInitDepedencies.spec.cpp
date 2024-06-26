// Copyright Andrei Sudarikov. All Rights Reserved.

#include "Misc/AutomationTest.h"

#include "DI/ObjectContainerBuilder.h"
#include "DI/ObjectContainer.h"

#include "MockClasses_BlueprintInitDependencies.h"
#include "BuildContainerHelper.h"

BEGIN_DEFINE_SPEC(FBlueprintInitDependenciesSpec, "UnrealDI.Blueprint InitDependencies", EAutomationTestFlags::ClientContext | EAutomationTestFlags::EditorContext | EAutomationTestFlags::ServerContext | EAutomationTestFlags::EngineFilter)
UObjectContainer* CreateContainer(const FString& ClassPath);
END_DEFINE_SPEC(FBlueprintInitDependenciesSpec)

void FBlueprintInitDependenciesSpec::Define()
{
    It("Should call InitDependencies with Object", [this]
    {
        UObjectContainer* Container = CreateContainer("/UnrealDITests/BP_TestInitDependencies_O.BP_TestInitDependencies_O_C");

        UTestBlueprintInitDependencies* Object = Container->Resolve<UTestBlueprintInitDependencies>();

        TestNotNull("Injected Reader", Object->Reader.GetInterface());
        TestNotNull("Injected Object", Object->DependencyObject.Get());
    });

    It("Should call InitDependencies with Interface", [this]
    {
        UObjectContainer* Container = CreateContainer("/UnrealDITests/BP_TestInitDependencies_I.BP_TestInitDependencies_I_C");

        UTestBlueprintInitDependencies* Object = Container->Resolve<UTestBlueprintInitDependencies>();

        TestNotNull("Injected Reader", Object->Reader.GetInterface());
        TestNotNull("Injected Interface", Object->DependencyInterface.GetInterface());
    });

    It("Should call InitDependencies on Derived class", [this]
    {
        UObjectContainer* Container = CreateContainer("/UnrealDITests/BP_TestInitDependencies_I_N_O.BP_TestInitDependencies_I_N_O_C");

        UTestBlueprintInitDependencies* Object = Container->Resolve<UTestBlueprintInitDependencies>();

        TestNotNull("Injected Reader", Object->Reader.GetInterface());
        TestNotNull("Injected Interface", Object->DependencyInterface.GetInterface());
        TestNotNull("Injected Object", Object->DependencyObject.Get());
    });

    It("Should call InitDependencies on class without Native base", [this]
    {
        FSoftObjectPath Path(TEXT("/UnrealDITests/BP_TestInitDependencies_Pure.BP_TestInitDependencies_Pure_C"));
        UClass* Class = (UClass*)Path.TryLoad();

        UObject* Object = NewObject<UObject>(GetTransientPackage(), Class);
        UObjectContainer* Container = CreateContainer("");

        Container->Inject(Object);

        bool bFoundObject = false;
        bool bFoundInterface = false;

        for (TFieldIterator<FProperty> It(Object->GetClass()); It; ++It)
        {
            if (It->GetName() == TEXT("DependencyObject"))
            {
                UObject* Value = *It->ContainerPtrToValuePtr<UObject*>(Object);

                TestNotNull("Injected Object", Value);
                TestTrue("Injected correct class", Value->IsA<UBlueprintDependencyObject>());
                bFoundObject = true;
            }
            else if (It->GetName() == TEXT("DependencyInterface"))
            {
                FScriptInterface Value = *It->ContainerPtrToValuePtr<FScriptInterface>(Object);

                TestNotNull("Injected Object", Value.GetObject());
                TestTrue("Injected correct class", Value.GetObject()->IsA<UBlueprintDependencyInterfaceImpl>());
                bFoundInterface = true;
            }
        }

        TestTrue("Found object", bFoundObject);
        TestTrue("Found interface", bFoundInterface);
    });
}

UObjectContainer* FBlueprintInitDependenciesSpec::CreateContainer(const FString& ClassPath)
{
    return FBuildContainerHelper::Build([&](FObjectContainerBuilder& Builder)
    {
        Builder.RegisterType<UBlueprintDependencyObject>();
        Builder.RegisterType<UBlueprintDependencyInterfaceImpl>().As<IBlueprintDependencyInterface>();

        if (!ClassPath.IsEmpty())
        {
            FSoftObjectPath Path(ClassPath);
            Builder.RegisterType<UTestBlueprintInitDependencies>().FromBlueprint(TSubclassOf<UTestBlueprintInitDependencies>((UClass*)Path.TryLoad()));
        }
    });
}
