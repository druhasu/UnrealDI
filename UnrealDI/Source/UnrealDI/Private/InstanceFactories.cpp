// Copyright Andrei Sudarikov. All Rights Reserved.

#include "DI/Impl/InstanceFactories.h"

#include "GameFramework/Actor.h"
#include "Blueprint/UserWidget.h"

/* UInstanceFactory_Object */

UObject* UInstanceFactory_Object::Create(UObject* Outer, UClass* EffectiveClass) const
{
    return NewObject<UObject>(Outer, EffectiveClass);
}


/* UInstanceFactory_Actor */

bool UInstanceFactory_Actor::IsClassSupported(UClass* EffectiveClass) const
{
    return EffectiveClass->IsChildOf<AActor>();
}

UObject* UInstanceFactory_Actor::Create(UObject* Outer, UClass* EffectiveClass) const
{
    UWorld* World = GetWorld();
    checkf(World, TEXT("Cannot retrieve World from container. Make sure you provided valid Outer to FObjectContainerBuilder::Build"));

    return World->SpawnActorDeferred<AActor>(EffectiveClass, FTransform::Identity);
}

void UInstanceFactory_Actor::FinalizeCreation(UObject* Object) const
{
    CastChecked<AActor>(Object)->FinishSpawning(FTransform::Identity);
}


/* UInstanceFactory_UserWidget */

bool UInstanceFactory_UserWidget::IsClassSupported(UClass* EffectiveClass) const
{
    return EffectiveClass->IsChildOf<UUserWidget>();
}

UObject* UInstanceFactory_UserWidget::Create(UObject* Outer, UClass* EffectiveClass) const
{
    UWorld* World = GetWorld();
    checkf(World, TEXT("Cannot retrieve World from container. Make sure you provided valid Outer to FObjectContainerBuilder::Build"));

    return CreateWidget<UUserWidget>(World, EffectiveClass);
}
