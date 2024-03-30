// Copyright Andrei Sudarikov. All Rights Reserved.

#include "Misc/AutomationTest.h"
#include "Tests/AutomationCommon.h"

#include "DI/ObjectContainerBuilder.h"
#include "DI/ObjectContainer.h"
#include "DI/Impl/DefaultInstanceFactory.h"

#include "TempWorldHelper.h"
#include "MockClasses_InjectOnConstruction.h"
#include "TestInstanceFactory.h"

BEGIN_DEFINE_SPEC(FIInstanceFactorySpec, "UnrealDI.IInstanceFactory", EAutomationTestFlags::ClientContext | EAutomationTestFlags::EditorContext | EAutomationTestFlags::ServerContext | EAutomationTestFlags::EngineFilter)
END_DEFINE_SPEC(FIInstanceFactorySpec)

void FIInstanceFactorySpec::Define()
{
    It("Should create UObject", [this]
    {
        FTempWorldHelper Helper;
        UDefaultInstanceFactory* Factory = GetMutableDefault<UDefaultInstanceFactory>();

        TestTrue("Class is supported", Factory->IsClassSupported(UInjectObject::StaticClass()));

        UObject* Result = Factory->Create(Helper.World, UInjectObject::StaticClass());
        Factory->FinalizeCreation(Result);

        TestNotNull("Result", Result);
        TestEqual("Result outer", Result->GetOuter(), (UObject*)Helper.World);
        TestEqual("Result class", Result->GetClass(), UInjectObject::StaticClass());
    });

    It("Should create AActor", [this]
    {
        FTempWorldHelper Helper;
        UDefaultInstanceFactory* Factory = GetMutableDefault<UDefaultInstanceFactory>();

        TestTrue("Class is supported", Factory->IsClassSupported(AInjectActor::StaticClass()));

        UObject* Result = Factory->Create(Helper.World, AInjectActor::StaticClass());
        Factory->FinalizeCreation(Result);

        TestNotNull("Result", Result);
        TestEqual("Result outer", Result->GetOuter(), (UObject*)Helper.World->GetLevel(0));
        TestEqual("Result class", Result->GetClass(), AInjectActor::StaticClass());
    });

    It("Should create UUserWidget", [this]
    {
        FTempWorldHelper Helper;
        UDefaultInstanceFactory* Factory = GetMutableDefault<UDefaultInstanceFactory>();

        TestTrue("Class is supported", Factory->IsClassSupported(UInjectWidget::StaticClass()));

        UObject* Result = Factory->Create(Helper.World, UInjectWidget::StaticClass());
        Factory->FinalizeCreation(Result);

        TestNotNull("Result", Result);
        TestEqual("Result outer", Result->GetOuter(), (UObject*)Helper.World);
        TestEqual("Result class", Result->GetClass(), UInjectWidget::StaticClass());
    });

    It("Should use IInstanceFactory from single Container", [this]
    {
        UTestInstanceFactory* Factory = NewObject<UTestInstanceFactory>();

        FObjectContainerBuilder Builder;
        Builder.RegisterInstance(Factory).As<IInstanceFactory>();

        UObjectContainer* Container = Builder.Build();

        UTestInstanceFactoryObject* Resolved = Container->Resolve<UTestInstanceFactoryObject>();

        TestEqual("CreatedBy", Resolved->CreatedBy, Factory);
        TestTrue("Finalize called", Resolved->bFinalizeCalled);
    });

    It("Should use IInstanceFactory from parent container if registered only in parent Container", [this]
    {
        UTestInstanceFactory* Factory = NewObject<UTestInstanceFactory>();

        FObjectContainerBuilder Builder;
        Builder.RegisterInstance(Factory).As<IInstanceFactory>();

        UObjectContainer* Parent = Builder.Build();
        UObjectContainer* Nested = FObjectContainerBuilder().BuildNested(*Parent);

        UTestInstanceFactoryObject* Resolved = Nested->Resolve<UTestInstanceFactoryObject>();

        TestEqual("CreatedBy", Resolved->CreatedBy, Factory);
        TestTrue("Finalize called", Resolved->bFinalizeCalled);
    });

    It("Should use IInstanceFactory from nested container if registered in both parent and nested Containers", [this]
    {
        UTestInstanceFactory* ParentFactory = NewObject<UTestInstanceFactory>();
        UTestInstanceFactory* NestedFactory = NewObject<UTestInstanceFactory>();

        FObjectContainerBuilder ParentBuilder;
        ParentBuilder.RegisterInstance(ParentFactory).As<IInstanceFactory>();
        UObjectContainer* Parent = ParentBuilder.Build();

        FObjectContainerBuilder NestedBuilder;
        NestedBuilder.RegisterInstance(NestedFactory).As<IInstanceFactory>();
        UObjectContainer* Nested = NestedBuilder.BuildNested(*Parent);

        UTestInstanceFactoryObject* Resolved = Nested->Resolve<UTestInstanceFactoryObject>();

        TestEqual("CreatedBy", Resolved->CreatedBy, NestedFactory);
        TestTrue("Finalize called", Resolved->bFinalizeCalled);
    });
}
