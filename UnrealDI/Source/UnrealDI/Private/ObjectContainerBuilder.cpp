// Copyright Andrei Sudarikov. All Rights Reserved.

#include "DI/ObjectContainerBuilder.h"
#include "DI/ObjectContainer.h"
#include "DI/Impl/DefaultInjectorProvider.h"

UObjectContainer* FObjectContainerBuilder::Build(UObject* Outer)
{
    UObjectContainer* Container = Outer ? NewObject<UObjectContainer>(Outer) : NewObject<UObjectContainer>();

    AddRegistrationsToContainer(Container);

    return Container;
}

UObjectContainer* FObjectContainerBuilder::BuildNested(UObjectContainer& Parent)
{
    UObjectContainer* Container = NewObject<UObjectContainer>(&Parent);
    Container->Storage.ParentStorage = &Parent.Storage;

    AddRegistrationsToContainer(Container);

    return Container;
}

void FObjectContainerBuilder::AddRegistrationsToContainer(UObjectContainer* Container)
{
    using namespace UnrealDI_Impl;

    FRegistrationStorage& Storage = Container->Storage;

    if (Storage.ParentStorage == nullptr)
    {
        // add default InjectorProvider before user provided registrations so it may be overriden.
        // add only in Root container, so user doesn't have to add override in each nested container
        Storage.AddRegistration(UInjectorProvider::StaticClass(), UDefaultInjectorProvider::StaticClass(), MakeShared<FLifetimeHandler_WeakSingleInstance>());
    }

    // add user provided registrations
    for (auto& Registration : Registrations)
    {
        TSharedRef<FLifetimeHandler> LifetimeHandler = Registration->CreateLifetimeHandler();

        // if no interface types declared, register as itself
        if (Registration->InterfaceTypes.Num() == 0)
        {
            Storage.AddRegistration(Registration->ImplClass, Registration->EffectiveClassPtr, LifetimeHandler);
        }

        // register all interfaces that this type implements
        for (UClass* Interface : Registration->InterfaceTypes)
        {
            Storage.AddRegistration(Interface, Registration->ImplClass, LifetimeHandler);
        }
    }

    // register container itself as IResolver
    Storage.AddRegistration(UResolver::StaticClass(), {}, MakeShared<FLifetimeHandler_Instance>(Container));

    // register container itself as IInjector
    Storage.AddRegistration(UInjector::StaticClass(), {}, MakeShared<FLifetimeHandler_Instance>(Container));

    // finalize creation and let Storage to create its services
    Storage.InitServices();

    // resolve all classes that are marked with bAutoCreate
    for (auto& Registration : Registrations)
    {
        if (Registration->bAutoCreate)
        {
            UClass* ClassToResolve = Registration->InterfaceTypes.Num() > 0 ? Registration->InterfaceTypes[0] : Registration->ImplClass;
            Container->Resolve(ClassToResolve);
        }
    }
}
