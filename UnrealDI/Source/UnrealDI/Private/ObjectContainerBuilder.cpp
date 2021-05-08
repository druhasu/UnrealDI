// Copyright Andrei Sudarikov. All Rights Reserved.

#include "DI/ObjectContainerBuilder.h"
#include "DI/ObjectContainer.h"

UObjectContainer* FObjectContainerBuilder::Build(UObject* Outer)
{
    UObjectContainer* Container = Outer ? NewObject<UObjectContainer>(Outer) : NewObject<UObjectContainer>();
    Container->Owner = Container;

    AddRegistrationsToContainer(Container);

    return Container;
}

UObjectContainer* FObjectContainerBuilder::BuildNested(UObjectContainer& Parent)
{
    UObjectContainer* Container = NewObject<UObjectContainer>(&Parent);
    Container->Owner = Container;
    Container->ParentContainer = &Parent;

    AddRegistrationsToContainer(Container);

    return Container;
}

void FObjectContainerBuilder::AddRegistrationsToContainer(UObjectContainer* Container)
{
    for (auto& Registration : Registrations)
    {
        TSharedRef<UnrealDI_Impl::FLifetimeHandler> LifetimeHandler = Registration->CreateLifetimeHandler();

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

    // register container itself as IResolver
    Container->AddRegistration(IResolver::UClassType::StaticClass(), MakeShared<UnrealDI_Impl::FLifetimeHandler_Instance>(Container));
}