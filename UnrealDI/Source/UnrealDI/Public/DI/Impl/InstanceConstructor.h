// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "Templates/EnableIf.h"
#include "Engine/World.h"
#include "Components/ActorComponent.h"
#include "Blueprint/UserWidget.h"

namespace UnrealDI_Impl
{

    /* Base constructor for simple UObjects */
    struct FGenericConstructor
    {
        static UObject* Construct(UWorld* World, UObject* Outer, UClass* EffectiveClass)
        {
            return NewObject<UObject>(Outer, EffectiveClass);
        }

        static void PostInit(UObject* Object) {}
    };

    /* Base constructor for Actors */
    struct FActorConstructor
    {
        static UObject* Construct(UWorld* World, UObject* Outer, UClass* EffectiveClass)
        {
            return World->SpawnActorDeferred<AActor>(EffectiveClass, FTransform::Identity);
        }

        static void PostInit(UObject* Object)
        {
            Cast<AActor>(Object)->FinishSpawning(FTransform::Identity);
        }
    };

    /* Base constructor for UserWidgets */
    struct FUserWidgetConstructor
    {
        static UObject* Construct(UWorld* World, UObject* Outer, UClass* EffectiveClass)
        {
            return CreateWidget<UUserWidget>(World, EffectiveClass);
        }

        static void PostInit(UObject* Object) {}
    };


    /* Constructor for simple UObjects */
    template <typename T, typename = void>
    struct TInstanceConstructor : public FGenericConstructor {};

    /* Constructor for Actors */
    template <typename T>
    struct TInstanceConstructor<T, typename TEnableIf<TIsDerivedFrom<T, AActor>::Value>::Type> : public FActorConstructor {};

    /* Constructor for UserWidgets */
    template <typename T>
    struct TInstanceConstructor<T, typename TEnableIf<TIsDerivedFrom<T, UUserWidget>::Value>::Type> : public FUserWidgetConstructor {};
}