// Copyright Andrei Sudarikov. All Rights Reserved.

#include "DI/ObjectContainerBuilder.h"
#include "DI/ObjectContainer.h"

UObjectContainer* FObjectContainerBuilder::Build(UObject* Outer)
{
    UObjectContainer* Container = Outer ? NewObject<UObjectContainer>(Outer) : NewObject<UObjectContainer>();
    Container->OuterForNewObjects = OuterForNewObjects ? OuterForNewObjects : Container->GetOuter();

    AddRegistrationsToContainer(Container);

    return Container;
}

UObjectContainer* FObjectContainerBuilder::BuildNested(UObjectContainer& Parent)
{
    UObjectContainer* Container = NewObject<UObjectContainer>(&Parent);
    Container->OuterForNewObjects = OuterForNewObjects ? OuterForNewObjects : Parent.OuterForNewObjects.Get();
    Container->ParentContainer = &Parent;

    AddRegistrationsToContainer(Container);

    return Container;
}

void FObjectContainerBuilder::SetOuterForNewObjects(UObject* Outer)
{
    OuterForNewObjects = Outer;
}

void FObjectContainerBuilder::AddRegistrationsToContainer(UObjectContainer* Container)
{
    using namespace UnrealDI_Impl;

    // reserve memory for all registrations + 1 for container itself
    Container->AllLifetimes.Reserve(Registrations.Num() + 1);

    // add user provided registrations
    for (auto& Registration : Registrations)
    {
        FLifetimeHandler* LifetimeHandler = Registration->CreateLifetimeHandler();
        Container->AllLifetimes.Add(LifetimeHandler);

        // if no interface types declared, register as itself
        if (Registration->InterfaceTypes.Num() == 0)
        {
            Container->AddRegistration(Registration->ImplClass, LifetimeHandler);
        }

        // register all interfaces that this type implements
        for (UClass* Interface : Registration->InterfaceTypes)
        {
            Container->AddRegistration(Interface, LifetimeHandler);
        }
    }

    FLifetimeHandler* ContainerInstance = new FLifetimeHandler_Instance(Container);
    Container->AllLifetimes.Add(ContainerInstance);

    // register container itself as IResolver
    Container->AddRegistration(UResolver::StaticClass(), ContainerInstance);

    // register container itself as IInjector
    Container->AddRegistration(UInjector::StaticClass(), ContainerInstance);

    // register container itself as IInjectorProvider, if not customized in either self or parent
    auto [Resolver, _] = Container->FindResolver(UInjectorProvider::StaticClass());
    if (Resolver == nullptr || Cast<UObjectContainer>(Resolver->Lifetime->Get()) != nullptr)
    {
        Container->AddRegistration(UInjectorProvider::StaticClass(), ContainerInstance);
    }

    // finalize creation and let Container create its services
    Container->FinalizeCreation();

    // resolve all classes that are marked with bAutoCreate
    for (int32 Index = 0; Index < Registrations.Num(); ++Index)
    {
        if (Registrations[Index]->bAutoCreate)
        {
            Container->ResolveImpl(*Container->AllLifetimes[Index]);
        }
    }
}
