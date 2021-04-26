// Copyright Andrei Sudarikov. All Rights Reserved.

#include "DI/NewObjectInjector.h"
#include "DI/ObjectContainer.h"
#include "DI/IInjectOnConstruction.h"

#include "Engine/World.h"

FNewObjectInjector FNewObjectInjector::Instance{};

void FNewObjectInjector::SetContainerForWorld(class UWorld* World, class UObjectContainer* Container)
{
    check(World);
    check(Container);

    if (ContainerMap.Num() == 0)
    {
        // setup listener
        GUObjectArray.AddUObjectCreateListener(this);
    }

    ContainerMap.Emplace(World, Container);
}

void FNewObjectInjector::RemoveContainerFromWorld(class UWorld* World)
{
    check(World);

    ContainerMap.Remove(World);

    if (ContainerMap.Num() == 0)
    {
        GUObjectArray.RemoveUObjectCreateListener(this);
    }
}

void FNewObjectInjector::NotifyUObjectCreated(const class UObjectBase* Object, int32 Index)
{
    // ignore CDOs
    if ((Object->GetFlags() & RF_ClassDefaultObject) != 0)
    {
        return;
    }

    // check whether we need to inject into this object
    if (Object->GetClass()->ImplementsInterface(UInjectOnConstruction::StaticClass()))
    {
        // find which world it belongs to
        UObject* Outer = Object->GetOuter();

        // Outer itself may be a World
        UWorld* World = Cast<UWorld>(Outer);

        // if not - request World from Outer
        if (World == nullptr)
        {
            World = Outer->GetWorld();
        }

        if (ensureAlwaysMsgf(World != nullptr,
            TEXT("Object '%s' of class '%s' requested injection but has no world assigned. No injection will occur. Make sure you pass valid Outer into NewObject()"),
            *Object->GetFName().ToString(), *Object->GetClass()->GetName()))
        {
            // ok, this object has valid World, now we need to find its container
            UObjectContainer** ContainerPtr = ContainerMap.Find(World);

            if (ContainerPtr != nullptr)
            {
                // store info for next constructor call
                IInjectOnConstruction::NextConstructorContainer = *ContainerPtr;
                IInjectOnConstruction::NextExpectedObject = (IInjectOnConstruction*)((UObjectBaseUtility*)Object)->GetInterfaceAddress(UInjectOnConstruction::StaticClass());
                IInjectOnConstruction::NextUObject = Object;
            }
            else
            {
                // we does not use ensure above because Container may be not existent, e.g. in tests, when we call InitDependencies manually
                // but print it at least, so it may be noticed
                UE_LOG(LogTemp, Display,
                    TEXT("Object '%s' of class '%s' requested injection, but no container was registered for its World. Make sure to call SetContainerForWorld with required World and Container"),
                    *Object->GetFName().ToString(), *Object->GetClass()->GetName());
            }
        }
    }
}