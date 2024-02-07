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
    UObjectContainer* Container = FBuildContainerHelper::Build(Helper.World);

    FInjectOnConstruction::SetContainerForWorld(Helper.World, Container);

    TObj* Obj = ObjFactory(Helper.World);

    TestNotNull("Dependency not injected", Obj->Resolver.GetInterface());

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
}