// Copyright Andrei Sudarikov. All Rights Reserved.

#include "DI/ObjectContainerBuilder.h"
#include "DI/ObjectContainer.h"

UObjectContainer* FObjectContainerBuilder::Build(UObject* Outer)
{
    UObjectContainer* Container = Outer ? NewObject<UObjectContainer>(Outer) : NewObject<UObjectContainer>();
    Container->InitStorage(Container);

    AddRegistrationsToContainer(Container);

    return Container;
}

UObjectContainer* FObjectContainerBuilder::BuildNested(UObjectContainer& Parent)
{
    UObjectContainer* Container = NewObject<UObjectContainer>(&Parent);
    Container->InitStorage(Container);
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