// Copyright Andrei Sudarikov. All Rights Reserved.

#include "Misc/AutomationTest.h"
#include "Tests/AutomationCommon.h"

#include "DI/ObjectContainerBuilder.h"
#include "DI/ObjectContainer.h"

#include "MockClasses.h"
#include "BuildContainerHelper.h"
#include "MockClasses_InjectOnConstruction.h"
#include "LatentCommands.h"
#include "TempWorldHelper.h"

BEGIN_DEFINE_SPEC(InjectOnConstructionSpec, "UnrealDI.InjectOnConstruction", EAutomationTestFlags::ClientContext | EAutomationTestFlags::EditorContext | EAutomationTestFlags::ServerContext | EAutomationTestFlags::EngineFilter)

template <typename TObj>
void TestCommon(TFunction<TObj*(UWorld*)> ObjFactory)
{
    FTempWorldHelper Helper;
    UObjectContainer* RootContainer = FObjectContainerBuilder().Build();
    UObjectContainer* Container = FObjectContainerBuilder().BuildNested(*RootContainer);

    FInjectOnConstruction::SetContainerForWorld(Helper.World, Container);

    TObj* Obj = ObjFactory(Helper.World);

    TestEqual("Resolver", Obj->Resolver, TScriptInterface<IResolver>(Container));

    FInjectOnConstruction::ClearContainerForWorld(Helper.World);
}

END_DEFINE_SPEC(InjectOnConstructionSpec)

void InjectOnConstructionSpec::Define()
{
    It("Should Set Container for World", [this]()
    {
        FTempWorldHelper Helper;

        FObjectContainerBuilder Builder;
        UObjectContainer* Container = Builder.Build(Helper.World);

        FInjectOnConstruction::SetContainerForWorld(Helper.World, Container);
        
        TestEqual("Associated container", FInjectOnConstruction::GetContainerForWorld(Helper.World), Container);

        FInjectOnConstruction::ClearContainerForWorld(Helper.World);
    });

    It("Should ReSet Container for World", [this]()
    {
        FTempWorldHelper Helper;

        FObjectContainerBuilder Builder;
        UObjectContainer* Container = Builder.Build(Helper.World);
        UObjectContainer* Container2 = Builder.Build(Helper.World);

        FInjectOnConstruction::SetContainerForWorld(Helper.World, Container);
        FInjectOnConstruction::SetContainerForWorld(Helper.World, Container2);

        TestEqual("Associated container", FInjectOnConstruction::GetContainerForWorld(Helper.World), Container2);

        FInjectOnConstruction::ClearContainerForWorld(Helper.World);
    });

    It("Should Clear Container for World", [this]()
    {
        FTempWorldHelper Helper;

        FObjectContainerBuilder Builder;
        UObjectContainer* Container = Builder.Build(Helper.World);

        FInjectOnConstruction::SetContainerForWorld(Helper.World, Container);
        FInjectOnConstruction::ClearContainerForWorld(Helper.World);

        TestNull("Associated container", FInjectOnConstruction::GetContainerForWorld(Helper.World));
    });

    It("Should Return nullptr Container for Unknown World", [this]()
    {
        FTempWorldHelper Helper;

        TestNull("Associated container", FInjectOnConstruction::GetContainerForWorld(Helper.World));
    });

    It("Should Return nullptr Container for nullptr World", [this]()
    {
        TestNull("Associated container", FInjectOnConstruction::GetContainerForWorld(nullptr));
    });

    It("Should Inject Into UObject", [this]()
    {
        TestCommon<UInjectObject>([](UWorld* World)
        {
            return NewObject<UInjectObject>(World);
        });
    });

    It("Should Inject Into AActor", [this]()
    {
        TestCommon<AInjectActor>([](UWorld* World)
        {
            return World->SpawnActor<AInjectActor>();
        });
    });

    It("Should Inject Into UUserWidget", [this]()
    {
        TestCommon<UInjectWidget>([](UWorld* World)
        {
            return CreateWidget<UInjectWidget>(World);
        });
    });

    It("Should Inject Into UActorComponent", [this]()
    {
        TestCommon<UInjectComponent>([](UWorld* World)
        {
            AInjectActorWithComponent* Actor = World->SpawnActor<AInjectActorWithComponent>();
            return Actor->Component;
        });
    });

    It("Should use custom IInjectorProvider", [this]
    {
        FTempWorldHelper Helper;

        UObjectContainer* OtherContainer = FBuildContainerHelper::Build();
        UTestInjectorProvider* InjectorProvider = NewObject<UTestInjectorProvider>();
        InjectorProvider->Injector = OtherContainer;

        UObjectContainer* WorldContainer = FBuildContainerHelper::Build(Helper.World, [&](FObjectContainerBuilder& Builder)
        {
            // override injector used by FInjectOnConstruction
            Builder.RegisterInstance(InjectorProvider).As<IInjectorProvider>();
        });

        FInjectOnConstruction::SetContainerForWorld(Helper.World, WorldContainer);

        UInjectObject* Object = NewObject<UInjectObject>(Helper.World); // will call TryInitDependencies from constructor

        TestEqual("Resolver", Object->Resolver, TScriptInterface<IResolver>(OtherContainer));

        FInjectOnConstruction::ClearContainerForWorld(Helper.World);
    });

    It("Should use custom IInjectorProvider in Nested container from Parent container", [this]
    {
        FTempWorldHelper Helper;

        UObjectContainer* OtherContainer = FBuildContainerHelper::Build();
        UTestInjectorProvider* InjectorProvider = NewObject<UTestInjectorProvider>();
        InjectorProvider->Injector = OtherContainer;

        UObjectContainer* RootContainer = FBuildContainerHelper::Build(Helper.World, [&](FObjectContainerBuilder& Builder)
        {
            // override injector used by FInjectOnConstruction
            Builder.RegisterInstance(InjectorProvider).As<IInjectorProvider>();
        });

        UObjectContainer* WorldContainer = FObjectContainerBuilder().BuildNested(*RootContainer);

        FInjectOnConstruction::SetContainerForWorld(Helper.World, WorldContainer);

        UInjectObject* Object = NewObject<UInjectObject>(Helper.World); // will call TryInitDependencies from constructor

        TestEqual("Resolver", Object->Resolver, TScriptInterface<IResolver>(OtherContainer));

        FInjectOnConstruction::ClearContainerForWorld(Helper.World);
    });

    It("Should use default IInjectorProvider in Nested container from Nested container", [this]
    {
        FTempWorldHelper Helper;

        UObjectContainer* RootContainer = FBuildContainerHelper::Build(Helper.World);
        UObjectContainer* WorldContainer = FObjectContainerBuilder().BuildNested(*RootContainer);
        FInjectOnConstruction::SetContainerForWorld(Helper.World, WorldContainer);

        UInjectObject* Object = NewObject<UInjectObject>(Helper.World); // will call TryInitDependencies from constructor

        TestEqual("Resolver", Object->Resolver, TScriptInterface<IResolver>(WorldContainer));

        FInjectOnConstruction::ClearContainerForWorld(Helper.World);
    });
}
