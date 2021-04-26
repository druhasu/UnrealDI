// Copyright Andrei Sudarikov. All Rights Reserved.

#include "DI/ObjectContainerBuilder.h"
#include "DI/ObjectContainer.h"

UObjectContainer* FObjectContainerBuilder::Build(UObject* Outer)
{
    UObjectContainer* Ret = Outer ? NewObject<UObjectContainer>(Outer) : NewObject<UObjectContainer>();

    for (auto& Registration : Registrations)
    {
        TSharedRef<UnrealDI_Impl::FLifetimeHandler> LifetimeHandler = Registration->GetLifetimeHandler();

        // if no interface types declared, register as itself
        if (Registration->InterfaceTypes.Num() == 0)
        {
            Ret->AddRegistration(Registration->ImplClass, LifetimeHandler);
        }

        // register all interfaces that this type implements
        for (UClass* Interface : Registration->InterfaceTypes)
        {
            Ret->AddRegistration(Interface, LifetimeHandler);
        }
    }

    // register container itself as IResolver
    Ret->AddRegistration(IResolver::UClassType::StaticClass(), MakeShared<UnrealDI_Impl::FLifetimeHandler_Instance>(Ret));

    return Ret;
}

UObjectContainer* FObjectContainerBuilder::BuildNested(UObjectContainer& Parent)
{
    return NewObject<UObjectContainer>();
}