// Copyright Andrei Sudarikov. All Rights Reserved.

#include "DI/Impl/DefaultInstanceFactory.h"

#include "GameFramework/Actor.h"
#include "Blueprint/UserWidget.h"
#include "Misc/EngineVersionComparison.h"

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

#if UE_VERSION_OLDER_THAN(5,5,0)
    UObject* ExistingObject = StaticFindObjectFastInternal(EffectiveClass, Outer, NewObjectName, true, RF_NoFlags, IsInAsyncLoadingThread() ? EInternalObjectFlags::None : EInternalObjectFlags::AsyncLoading);
#else
    UObject* ExistingObject = StaticFindObjectFastInternal(EffectiveClass, Outer, NewObjectName, true, RF_NoFlags, IsInAsyncLoadingThread() ? EInternalObjectFlags::None : EInternalObjectFlags_AsyncLoading);
#endif
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
