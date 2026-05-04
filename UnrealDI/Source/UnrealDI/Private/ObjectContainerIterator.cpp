// Copyright Andrei Sudarikov. All Rights Reserved.

#include "DI/ObjectContainerIterator.h"
#include "DI/ObjectContainer.h"
#include "DI/Impl/Lifetimes.h"

namespace UnrealDI_Impl
{

FScriptMapLayout FObjectContainerIteratorBase::MapLayout = FScriptMap::GetScriptLayout(
    sizeof(UClass*), alignof(UClass*),
    sizeof(UObjectContainer::FResolversArray), alignof(UObjectContainer::FResolversArray)
);

FObjectContainerIteratorBase::FObjectContainerIteratorBase(TConstArrayView<UObjectContainer*> InContainers, UClass* InClass)
    : Containers(InContainers)
    , DesiredClass(InClass)
    , ContainerIndex(0)
    , MapIndex(0)
    , ArrayIndex(-1)
{
    Advance();
}

UObject* FObjectContainerIteratorBase::Get(bool bCreateIfNotExists) const
{
    checkf(HasValue(), TEXT("You should not access iterator when it has no value"));

    const UObjectContainer* Container = Containers[ContainerIndex];

    FScriptMap* Map = (FScriptMap*)&Container->Registrations;
    auto* Pair = (TPair<UClass*, UObjectContainer::FResolversArray>*)Map->GetData(MapIndex, MapLayout);
    const UObjectContainer::FResolver& Resolver = Pair->Value[ArrayIndex];

    UObject* Result = Resolver.Lifetime->Get();
    if (Result == nullptr && bCreateIfNotExists)
    {
        Result = Container->ResolveImpl(*Resolver.Lifetime);
    }

    return Result;
}

void FObjectContainerIteratorBase::Advance()
{
    if (!HasValue())
    {
        return;
    }

    ArrayIndex += 1;

    while (ContainerIndex  < Containers.Num())
    {
        const UObjectContainer* Container = Containers[ContainerIndex];
        FScriptMap* Map = (FScriptMap*)&Container->Registrations;

        while (MapIndex < Map->GetMaxIndex())
        {
            // Map may have invalid indexes in the middle, so we skip them
            if (!Map->IsValidIndex(MapIndex))
            {
                MapIndex += 1;
                continue;
            }

            auto* Pair = (TPair<UClass*, UObjectContainer::FResolversArray>*)Map->GetData(MapIndex, MapLayout);
            if (ArrayIndex == 0 && !Pair->Key->IsChildOf(DesiredClass))
            {
                MapIndex += 1;
                continue;
            }

            if (Pair->Value.IsValidIndex(ArrayIndex))
            {
                return;
            }

            MapIndex += 1;
            ArrayIndex = 0;
        }

        ContainerIndex += 1;
        MapIndex = 0;
        ArrayIndex = 0;
    }

    // break iteration, make HasValue return false
    ContainerIndex = -1;
}

}
