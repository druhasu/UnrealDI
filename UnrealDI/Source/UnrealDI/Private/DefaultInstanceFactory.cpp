// Copyright Andrei Sudarikov. All Rights Reserved.

#include "DI/Impl/DefaultInstanceFactory.h"

#include "GameFramework/Actor.h"
#include "Blueprint/UserWidget.h"

UObject* UDefaultInstanceFactory::Create(UObject* Outer, UClass* EffectiveClass) const
{
    UWorld* World = Outer->GetWorld();

    if (EffectiveClass->IsChildOf<AActor>())
    {
        checkf(World != nullptr, TEXT("Cannot retrieve World from container. Make sure you provided valid Outer to FObjectContainerBuilder::Build"));

        return World->SpawnActorDeferred<AActor>(EffectiveClass, FTransform::Identity);
    }

    if (EffectiveClass->IsChildOf<UUserWidget>())
    {
        checkf(World != nullptr, TEXT("Cannot retrieve World from container. Make sure you provided valid Outer to FObjectContainerBuilder::Build"));

        return CreateWidget<UUserWidget>(World, EffectiveClass);
    }

    // try to create objects with stable names if possible
    FName NewObjectName = EffectiveClass->GetFName();

    UObject* ExistingObject = StaticFindObjectFastInternal(EffectiveClass, Outer, NewObjectName, true, RF_NoFlags, IsInAsyncLoadingThread() ? EInternalObjectFlags::None : EInternalObjectFlags::AsyncLoading);
    if (ExistingObject != nullptr)
    {
        // object with this name already exists, fallback to unique name
        NewObjectName = MakeUniqueObjectName(Outer, EffectiveClass, NewObjectName);
    }

    return NewObject<UObject>(Outer, EffectiveClass, NewObjectName);
}

void UDefaultInstanceFactory::FinalizeCreation(UObject* Object) const
{
    if (AActor* Actor = Cast<AActor>(Object))
    {
        Actor->FinishSpawning(FTransform::Identity);
    }
}
