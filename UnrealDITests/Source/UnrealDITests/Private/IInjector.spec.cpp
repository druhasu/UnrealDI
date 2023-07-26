// Copyright Andrei Sudarikov. All Rights Reserved.

#include "Misc/AutomationTest.h"

#include "DI/ObjectContainerBuilder.h"
#include "DI/ObjectContainer.h"

#include "MockClasses_Injector.h"

BEGIN_DEFINE_SPEC(IInjectorSpec, "UnrealDI.IInjector", EAutomationTestFlags::ClientContext | EAutomationTestFlags::EditorContext | EAutomationTestFlags::ServerContext | EAutomationTestFlags::EngineFilter)
TScriptInterface<IInjector> CreateInjector()
{
    FObjectContainerBuilder Builder;
    Builder.RegisterType<UMockReader>().As<IReader>().AsSelf();
    Builder.RegisterInjector<UTestInjection_Base>();
    return Builder.Build()->Resolve<IInjector>();
}
END_DEFINE_SPEC(IInjectorSpec)

void IInjectorSpec::Define()
{
    It("Should Resolve Injector", [this]
    {
        auto Injector = CreateInjector();

        TestNotNull("Injector", Injector.GetObject());
    });

    It("Should Inject into Exact Type", [this]
    {
        UTestInjection_Base* TargetObject = NewObject<UTestInjection_Base>();
        auto Injector = CreateInjector();

        bool bCanInject = Injector->CanInject(TargetObject->GetClass());
        bool bResult = Injector->Inject(TargetObject);

        TestTrue("CanInject", bCanInject);
        TestTrue("Was injected", bResult);
        TestNotNull("Injected instance", TargetObject->Instance.GetObject());
    });

    It("Should Inject into Derived Type", [this]
    {
        UTestInjection_Derived* TargetObject = NewObject<UTestInjection_Derived>();
        auto Injector = CreateInjector();

        bool bCanInject = Injector->CanInject(TargetObject->GetClass());
        bool bResult = Injector->Inject(TargetObject);

        TestTrue("CanInject", bCanInject);
        TestTrue("Was injected", bResult);
        TestNotNull("Injected instance", TargetObject->Instance.GetObject());
    });

    It("Should Inject into Static Exposed Type", [this]
    {
        UTestInjection_Exposed* TargetObject = NewObject<UTestInjection_Exposed>();
        auto Injector = CreateInjector();

        bool bCanInject = Injector->CanInject(TargetObject->GetClass());
        bool bResult = Injector->Inject(TargetObject);

        TestTrue("CanInject", bCanInject);
        TestTrue("Was injected", bResult);
        TestNotNull("Injected instance", TargetObject->Instance.GetObject());
    });

    It("Should Inject into Blueprint Derived Type", [this]
    {
        TSoftClassPtr<UTestInjection_Base> ClassPath(FSoftObjectPath("/UnrealDITests/BP_TestInjection_Derived.BP_TestInjection_Derived_C"));
        UTestInjection_Base* TargetObject = NewObject<UTestInjection_Base>(GetTransientPackage(), ClassPath.LoadSynchronous());
        auto Injector = CreateInjector();

        bool bCanInject = Injector->CanInject(TargetObject->GetClass());
        bool bResult = Injector->Inject(TargetObject);

        TestTrue("CanInject", bCanInject);
        TestTrue("Was injected", bResult);
        TestNotNull("Injected instance", TargetObject->Instance.GetObject());
    });

    It("Should Return false When Not Injected", [this]
    {
        UTestInjection_Unregistered* TargetObject = NewObject<UTestInjection_Unregistered>();
        auto Injector = CreateInjector();

        bool bCanInject = Injector->CanInject(TargetObject->GetClass());
        bool bResult = Injector->Inject(TargetObject);

        TestFalse("CanInject", bCanInject);
        TestFalse("Was injected", bResult);
        TestNull("Injected instance", TargetObject->Instance.GetObject());
    });
}