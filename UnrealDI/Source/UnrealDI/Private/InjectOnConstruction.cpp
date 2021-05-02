// Copyright Andrei Sudarikov. All Rights Reserved.

#include "DI/InjectOnConstruction.h"

TMap<UWorld*, UObjectContainer*> FInjectOnConstruction::ContainerMap{};

void FInjectOnConstruction::SetContainerForWorld(class UWorld* World, class UObjectContainer* Container)
{
    check(World);
    check(Container);

    ContainerMap.Emplace(World, Container);
}

void FInjectOnConstruction::ClearContainerForWorld(class UWorld* World)
{
    check(World);

    ContainerMap.Remove(World);
}