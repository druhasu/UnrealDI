// Copyright Andrei Sudarikov. All Rights Reserved.

#include "DI/InjectOnConstruction.h"
#include "DI/IInjector.h"
#include "DI/IInjectorProvider.h"
#include "DI/ObjectContainer.h"
#include "Engine/World.h"

void FInjectOnConstruction::SetContainerForWorld(UWorld* World, class UObjectContainer* Container)
{
    check(World);
    check(Container);

    ClearContainerForWorld(World);
    World->PerModuleDataObjects.Add(Container);
}

void FInjectOnConstruction::ClearContainerForWorld(UWorld* World)
{
    check(World);

    World->PerModuleDataObjects.RemoveAllSwap([&](TObjectPtr<UObject> Obj) { return Obj->IsA<UObjectContainer>(); });
}

UObjectContainer* FInjectOnConstruction::GetContainerForWorld(UWorld* World)
{
    UObjectContainer* Result = nullptr;
    if (World)
    {
        World->PerModuleDataObjects.FindItemByClass(&Result);
    }
    return Result;
}

void FInjectOnConstruction::TryInitDependencies(UObject* Self)
{
    check(Self != nullptr);

    TryInitDependenciesInternal(Self);
}

void FInjectOnConstruction::TryInitDependenciesInternal(UObject* Self)
{
    UWorld* World = Self->GetWorld();
    UObjectContainer* Container = GetContainerForWorld(World);

    if (Container)
    {
        TScriptInterface<IInjectorProvider> Provider = Container->Resolve<IInjectorProvider>();

        TScriptInterface<IInjector> Injector = Provider->GetInjector(Self);
        if (Injector != nullptr)
        {
            Injector->Inject(Self);
        }
    }
}
