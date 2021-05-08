// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "DI/Impl/InitDependenciesInvoker.h"
#include "DI/Impl/ArgumentPack.h"
#include "DI/Impl/InitMethodTypologyDeducer.h"
#include "DI/Impl/IsSupportedArgument.h"
#include "DI/Impl/InstanceFactoryResult.h"
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
         * Provides static CreateFactory method.
         * CreateFactory will return TFunction that creates instance of type T with arguments from TArgumentPack
         */
        template <typename T, typename TArgumentPack, typename = void>
        struct TInstanceFactory;

        // specialization for UObject without dependencies
        template <typename T>
        struct TInstanceFactory<T, TArgumentPack<>, typename TEnableObjectFactory<T>::Type >
        {
            static FInstanceFactoryResult CreateFactory(UClass* EffectiveClass)
            {
                check(EffectiveClass);

                return FInstanceFactoryResult
                {
                    [EffectiveClass](FRegistrationStorage& Resolver)
                    {
                        return NewObject<T>(Resolver.GetOwner(), EffectiveClass);
                    },
                    EffectiveClass
                };
            }
        };

        // specialization for UObject with dependencies
        template <typename T, typename... TArgs>
        struct TInstanceFactory<T, TArgumentPack<TArgs...>, typename TEnableObjectFactory<T>::Type >
        {
            static FInstanceFactoryResult CreateFactory(UClass* EffectiveClass)
            {
                check(EffectiveClass);

                return FInstanceFactoryResult
                {
                    [EffectiveClass](FRegistrationStorage& Resolver)
                    {
                        T* Ret = NewObject<T>(Resolver.GetOwner(), EffectiveClass);
                        TInitDependenciesInvoker<T, TArgumentPack<TArgs...>>::Invoke(Ret, Resolver);
                        return Ret;
                    },
                    EffectiveClass
                };
            }
        };

        // specialization for AActor without dependencies
        template <typename T>
        struct TInstanceFactory<T, TArgumentPack<>, typename TEnableActorFactory<T>::Type >
        {
            static FInstanceFactoryResult CreateFactory(UClass* EffectiveClass)
            {
                check(EffectiveClass);

                return FInstanceFactoryResult
                {
                    [EffectiveClass](FRegistrationStorage& Resolver)
                    {
                        UWorld* World = Resolver.GetWorld();
                        checkf(World, TEXT("Cannot retrive World from container. Make sure you provided valid Outer to FObjectContainerBuilder::Build"));
                        return World->SpawnActor<T>(EffectiveClass, FTransform::Identity);
                    },
                    EffectiveClass
                };
            }
        };

        // specialization for AActor with dependencies
        template <typename T, typename... TArgs>
        struct TInstanceFactory<T, TArgumentPack<TArgs...>, typename TEnableActorFactory<T>::Type >
        {
            static FInstanceFactoryResult CreateFactory(UClass* EffectiveClass)
            {
                check(EffectiveClass);

                return FInstanceFactoryResult
                {
                    [EffectiveClass](FRegistrationStorage& Resolver)
                    {
                        UWorld* World = Resolver.GetWorld();
                        checkf(World, TEXT("Cannot retrive World from container. Make sure you provided valid Outer to FObjectContainerBuilder::Build"));
                        T* Ret = World->SpawnActorDeferred<T>(EffectiveClass, FTransform::Identity);
                        TInitDependenciesInvoker<T, TArgumentPack<TArgs...>>::Invoke(Ret, Resolver);
                        Ret->FinishSpawning(FTransform::Identity);
                        return Ret;
                    },
                    EffectiveClass
                };
            }
        };

        // specialization for UUserWidget without dependencies
        template <typename T>
        struct TInstanceFactory<T, TArgumentPack<>, typename TEnableWidgetFactory<T>::Type >
        {
            static FInstanceFactoryResult CreateFactory(UClass* EffectiveClass)
            {
                return FInstanceFactoryResult
                {
                    [EffectiveClass](FRegistrationStorage& Resolver)
                    {
                        check(EffectiveClass);
                        UWorld* World = Resolver.GetWorld();
                        checkf(World, TEXT("Cannot retrive World from container. Make sure you provided valid Outer to FObjectContainerBuilder::Build"));
                        return CreateWidget<T>(World, EffectiveClass);
                    },
                    EffectiveClass
                };
            }
        };

        // specialization for UUserWidget with dependencies
        template <typename T, typename... TArgs>
        struct TInstanceFactory<T, TArgumentPack<TArgs...>, typename TEnableWidgetFactory<T>::Type >
        {
            static FInstanceFactoryResult CreateFactory(UClass* EffectiveClass)
            {
                return FInstanceFactoryResult
                {
                    [EffectiveClass](FRegistrationStorage& Resolver)
                    {
                        check(EffectiveClass);
                        UWorld* World = Resolver.GetWorld();
                        checkf(World, TEXT("Cannot retrive World from container. Make sure you provided valid Outer to FObjectContainerBuilder::Build"));
                        T* Ret = CreateWidget<T>(World, EffectiveClass);
                        TInitDependenciesInvoker<T, TArgumentPack<TArgs...>>::Invoke(Ret, Resolver);
                        return Ret;
                    },
                    EffectiveClass
                };
            }
        };
    }

    /*
     * Provides static CreateFactory method.
     * CreateFactory will return TFunction that creates instance of type T with required dependencies
     */
    template <typename T>
    using TInstanceFactory = Details::TInstanceFactory< T, TInitMethodTypologyDeducer< T > >;
}