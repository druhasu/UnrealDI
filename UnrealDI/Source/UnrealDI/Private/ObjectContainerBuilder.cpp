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

    // add user provided registrations
    for (auto& Registration : Registrations)
    {
        TSharedRef<FLifetimeHandler> LifetimeHandler = Registration->CreateLifetimeHandler();

        // if no interface types declared, register as itself
        if (Registration->InterfaceTypes.Num() == 0)
        {
            Container->AddRegistration(Registration->ImplClass, Registration->EffectiveClassPtr, LifetimeHandler);
        }

        // register all interfaces that this type implements
        for (UClass* Interface : Registration->InterfaceTypes)
        {
            Container->AddRegistration(Interface, Registration->ImplClass, LifetimeHandler);
        }
    }

    // register container itself as IResolver
    Container->AddRegistration(UResolver::StaticClass(), {}, MakeShared<FLifetimeHandler_Instance>(Container));

    // register container itself as IInjector
    Container->AddRegistration(UInjector::StaticClass(), {}, MakeShared<FLifetimeHandler_Instance>(Container));

    // register container itself as IInjectorProvider, if not customized in either self or parent
    auto [Resolver, _] = Container->FindResolver(UInjectorProvider::StaticClass());
    if (Resolver == nullptr || Resolver->EffectiveClass.IsNull())
    {
        Container->AddRegistration(UInjectorProvider::StaticClass(), {}, MakeShared<FLifetimeHandler_Instance>(Container));
    }

    // finalize creation and let Container create its services
    Container->InitServices();

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
