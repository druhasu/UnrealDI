// Copyright Andrei Sudarikov. All Rights Reserved.

#include "Misc/AutomationTest.h"
#include "Tests/AutomationCommon.h"

#include "DI/ObjectContainerBuilder.h"
#include "DI/ObjectContainer.h"

#include "MockClasses.h"
#include "MockReader.h"
#include "LatentCommands.h"

BEGIN_DEFINE_SPEC(LifetimesSpec, "UnrealDI.Lifetimes", EAutomationTestFlags::ClientContext | EAutomationTestFlags::EditorContext | EAutomationTestFlags::ServerContext | EAutomationTestFlags::EngineFilter)
END_DEFINE_SPEC(LifetimesSpec)

template <typename T>
struct CreateListener : public FUObjectArray::FUObjectCreateListener
{
    CreateListener() { GUObjectArray.AddUObjectCreateListener(this); }
    ~CreateListener() { GUObjectArray.RemoveUObjectCreateListener(this); }
    void NotifyUObjectCreated(const UObjectBase* Object, int32 Index)
    {
        WasCreated |= Object->GetClass() == T::StaticClass();
    }
    void OnUObjectArrayShutdown() {}

    bool WasCreated = false;
};

void LifetimesSpec::Define()
{
    Describe("Transient", [this]()
    {
        It("Should Resolve", [this]()
        {
            FObjectContainerBuilder Builder;
            Builder.RegisterType<UMockReader>();
            UObjectContainer* Container = Builder.Build();

            UObject* ResolvedObject = Container->Resolve(UMockReader::StaticClass());
            UMockReader* Reader = Cast<UMockReader>(ResolvedObject);

            TestNotNull("Resolve returned null", ResolvedObject);
            TestNotNull("Resolve returned wrong type", Reader);
        });

        It("Should Resolve New Objects", [this]()
        {
            FObjectContainerBuilder Builder;
            Builder.RegisterType<UMockReader>();
            UObjectContainer* Container = Builder.Build();

            UMockReader* Reader1 = Container->Resolve<UMockReader>();
            UMockReader* Reader2 = Container->Resolve<UMockReader>();

            TestNotEqual("Resolve returned same objects", Reader1, Reader2);
        });
    });

    Describe("SingleInstance", [this]()
    {
        It("Should Resolve", [this]()
        {
            FObjectContainerBuilder Builder;
            Builder.RegisterType<UMockReader>().SingleInstance();
            UObjectContainer* Container = Builder.Build();

            UObject* ResolvedObject = Container->Resolve(UMockReader::StaticClass());
            UMockReader* Reader = Cast<UMockReader>(ResolvedObject);

            TestNotNull("Resolve returned null", ResolvedObject);
            TestNotNull("Resolve returned wrong type", Reader);
        });

        It("Should Resolve Same Object", [this]()
        {
            FObjectContainerBuilder Builder;
            Builder.RegisterType<UMockReader>().SingleInstance();
            UObjectContainer* Container = Builder.Build();

            UMockReader* Reader1 = Container->Resolve<UMockReader>();
            UMockReader* Reader2 = Container->Resolve<UMockReader>();

            TestEqual("Resolve returned different objects", Reader1, Reader2);
        });

        It("Should Survive GC", [this]()
        {
            FObjectContainerBuilder Builder;
            Builder.RegisterType<UMockReader>().SingleInstance();
            UObjectContainer* Container = Builder.Build();
            Container->AddToRoot();

            // call resolve to initially construct an object
            UMockReader* Reader1 = Container->Resolve<UMockReader>();

            ADD_LATENT_AUTOMATION_COMMAND(FRunGC);
            ADD_LATENT_AUTOMATION_COMMAND(FFunctionLatentCommand([this, Container, Reader1]()
            {
                UMockReader* Reader2 = Container->Resolve<UMockReader>();

                TestNotNull("Resolve returned nullptr", Reader2);
                TestTrue("Resolve returned invalid object", Reader2->IsValidLowLevel());
                TestEqual("Resolve returned different objects", Reader1, Reader2);
                Container->RemoveFromRoot();
                return true;
            }));
        });

        It("Should Create Instance After Build If AutoCreate=true", [this]()
        {
            FObjectContainerBuilder Builder;
            Builder.RegisterType<UMockReader>().SingleInstance(true);
            
            CreateListener<UMockReader> Listener;
            UObjectContainer* Container = Builder.Build();

            TestTrue("Object was not created", Listener.WasCreated);
        });

        It("Should Not Create Instance After Build If AutoCreate=false", [this]()
        {
            FObjectContainerBuilder Builder;
            Builder.RegisterType<UMockReader>().SingleInstance(/* false */);

            CreateListener<UMockReader> Listener;
            UObjectContainer* Container = Builder.Build();

            TestFalse("Object was created", Listener.WasCreated);
        });
    });

    Describe("WeakSingleInstance", [this]()
    {
        It("Should Resolve", [this]()
        {
            FObjectContainerBuilder Builder;
            Builder.RegisterType<UMockReader>().WeakSingleInstance();
            UObjectContainer* Container = Builder.Build();

            UObject* ResolvedObject = Container->Resolve(UMockReader::StaticClass());
            UMockReader* Reader = Cast<UMockReader>(ResolvedObject);

            TestNotNull("Resolve returned nullptr", ResolvedObject);
            TestNotNull("Resolve returned wrong type", Reader);
        });

        It("Should Resolve Same Object", [this]()
        {
            FObjectContainerBuilder Builder;
            Builder.RegisterType<UMockReader>().WeakSingleInstance();
            UObjectContainer* Container = Builder.Build();

            UMockReader* Reader1 = Container->Resolve<UMockReader>();
            UMockReader* Reader2 = Container->Resolve<UMockReader>();

            TestEqual("Resolve returned different objects", Reader1, Reader2);
        });

        It("Should Not Survive GC", [this]()
        {
            FObjectContainerBuilder Builder;
            Builder.RegisterType<UMockReader>().WeakSingleInstance();
            UObjectContainer* Container = Builder.Build();
            Container->AddToRoot();

            // call resolve to initially construct an object
            TWeakObjectPtr<UMockReader> Reader1 = Container->Resolve<UMockReader>();

            ADD_LATENT_AUTOMATION_COMMAND(FRunGC);
            ADD_LATENT_AUTOMATION_COMMAND(FFunctionLatentCommand([this, Container, Reader1]()
            {
                UMockReader* Reader2 = Container->Resolve<UMockReader>();

                TestNotNull("Resolve returned nullptr", Reader2);
                TestNull("Previous instance was not collected", Reader1.Get());
                Container->RemoveFromRoot();
                return true;
            }));
        });
    });

    Describe("Instance", [this]()
    {
        It("Should Resolve", [this]()
        {
            FObjectContainerBuilder Builder;
            Builder.RegisterInstance<UMockReader>(NewObject<UMockReader>());
            UObjectContainer* Container = Builder.Build();

            UObject* ResolvedObject = Container->Resolve<UMockReader>();
            UMockReader* Reader = Cast<UMockReader>(ResolvedObject);

            TestNotNull("Resolve returned nullptr", ResolvedObject);
            TestNotNull("Resolve returned wrong type", Reader);
        });

        It("Should Resolve Same Object", [this]()
        {
            UMockReader* Reader1 = NewObject<UMockReader>();

            FObjectContainerBuilder Builder;
            Builder.RegisterInstance<UMockReader>(Reader1);
            UObjectContainer* Container = Builder.Build();

            UMockReader* Reader2 = Container->Resolve<UMockReader>();

            TestEqual("Resolve returned different objects", Reader1, Reader2);
        });

        It("Should Survive GC", [this]()
        {
            UMockReader* Reader1 = NewObject<UMockReader>();

            FObjectContainerBuilder Builder;
            Builder.RegisterInstance<UMockReader>(Reader1);
            UObjectContainer* Container = Builder.Build();
            Container->AddToRoot();

            ADD_LATENT_AUTOMATION_COMMAND(FRunGC);
            ADD_LATENT_AUTOMATION_COMMAND(FFunctionLatentCommand([this, Container, Reader1]()
            {
                UMockReader* Reader2 = Container->Resolve<UMockReader>();

                TestNotNull("Resolve returned nullptr", Reader2);
                TestTrue("Resolve returned invalid object", Reader2->IsValidLowLevel());
                TestEqual("Resolve returned different objects", Reader1, Reader2);
                Container->RemoveFromRoot();
                return true;
            }));
        });
    });
}