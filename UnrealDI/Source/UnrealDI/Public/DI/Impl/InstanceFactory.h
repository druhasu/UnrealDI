// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "DI/Impl/InitDependenciesInvoker.h"
#include "DI/Impl/ArgumentPack.h"
#include "DI/Impl/InitMethodTypologyDeducer.h"
#include "DI/Impl/DependenciesRegistry.h"
#include "Templates/EnableIf.h"
#include "Blueprint/UserWidget.h"

namespace UnrealDI_Impl
{
    class FRegistrationStorage;

    namespace Details
    {
        // should we construct with NewObject ?
        template <typename T>
        struct TEnableObjectFactory : TEnableIf< TAnd< TNot<TIsDerivedFrom<T, AActor> >, TNot< TIsDerivedFrom<T, UUserWidget> > >::Value > {};

        // should we construct with SpawnActor ?
        template <typename T>
        struct TEnableActorFactory : TEnableIf< TIsDerivedFrom<T, AActor>::Value > {};

        // should we construct with CreateWidget ?
        template <typename T>
        struct TEnableWidgetFactory : TEnableIf< TIsDerivedFrom<T, UUserWidget>::Value > {};

        /*
         * Provides static CreateInstance method.
         * CreateInstance will create instance of type T with dependenices based on arguments from TArgumentPack
         */
        template <typename T, typename = void>
        struct TInstanceFactory;

        // specialization for UObject
        template <typename T>
        struct TInstanceFactory<T, typename TEnableObjectFactory<T>::Type >
        {
            static UObject* CreateInstance(FRegistrationStorage& Resolver, UClass* EffectiveClass)
            {
                T* Ret = NewObject<T>(Resolver.GetOuterForNewObject(), EffectiveClass);
                if (auto Func = FDependenciesRegistry::FindInitFunction(EffectiveClass))
                {
                    Func(*Ret, Resolver);
                }
                return Ret;
            }
        };

        // specialization for AActor
        template <typename T>
        struct TInstanceFactory<T, typename TEnableActorFactory<T>::Type >
        {
            static UObject* CreateInstance(FRegistrationStorage& Resolver, UClass* EffectiveClass)
            {
                UWorld* World = Resolver.GetWorld();
                checkf(World, TEXT("Cannot retrieve World from container. Make sure you provided valid Outer to FObjectContainerBuilder::Build"));
                T* Ret = World->SpawnActorDeferred<T>(EffectiveClass, FTransform::Identity);
                if (auto Func = FDependenciesRegistry::FindInitFunction(EffectiveClass))
                {
                    Func(*Ret, Resolver);
                }
                Ret->FinishSpawning(FTransform::Identity);
                return Ret;
            }
        };

        // specialization for UUserWidget
        template <typename T>
        struct TInstanceFactory<T, typename TEnableWidgetFactory<T>::Type >
        {
            static UObject* CreateInstance(FRegistrationStorage& Resolver, UClass* EffectiveClass)
            {
                UWorld* World = Resolver.GetWorld();
                checkf(World, TEXT("Cannot retrieve World from container. Make sure you provided valid Outer to FObjectContainerBuilder::Build"));
                T* Ret = CreateWidget<T>(World, EffectiveClass);
                if (auto Func = FDependenciesRegistry::FindInitFunction(EffectiveClass))
                {
                    Func(*Ret, Resolver);
                }
                return Ret;
            }
        };
    }

    /*
     * Provides static CreateInstance method.
     * CreateInstance will return instance of type T with required dependencies
     */
    template <typename T>
    using TInstanceFactory = Details::TInstanceFactory< T >;
}