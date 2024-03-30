// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "IReader.h"
#include "GameFramework/Actor.h"
#include "Blueprint/UserWidget.h"
#include "DI/InjectOnConstruction.h"
#include "DI/IInjectorProvider.h"
#include "DI/IResolver.h"
#include "MockClasses_InjectOnConstruction.generated.h"

/* UObject subclass of FInjectOnConstruction */
UCLASS()
class UNREALDITESTS_API UInjectObject : public UObject, public FInjectOnConstruction
{
    GENERATED_BODY()
public:
    UInjectObject()
    {
        TryInitDependencies(this);
    }

    void InitDependencies(TScriptInterface<IResolver>&& InResolver)
    {
        Resolver = MoveTemp(InResolver);
    }

    UPROPERTY()
    TScriptInterface<IResolver> Resolver;
};

/* AActor subclass of FInjectOnConstruction */
UCLASS()
class UNREALDITESTS_API AInjectActor : public AActor, public FInjectOnConstruction
{
    GENERATED_BODY()
public:
    AInjectActor()
    {
        TryInitDependencies(this);
    }

    void InitDependencies(TScriptInterface<IResolver>&& InResolver)
    {
        Resolver = MoveTemp(InResolver);
    }

    UPROPERTY()
    TScriptInterface<IResolver> Resolver;
};

/* UUserWidget subclass of FInjectOnConstruction */
UCLASS()
class UNREALDITESTS_API UInjectWidget : public UUserWidget, public FInjectOnConstruction
{
    GENERATED_BODY()
public:
    UInjectWidget(const FObjectInitializer& ObjectInitializer)
        : Super(ObjectInitializer)
    {
        TryInitDependencies(this);
    }

    void InitDependencies(TScriptInterface<IResolver>&& InResolver)
    {
        Resolver = MoveTemp(InResolver);
    }

    UPROPERTY()
    TScriptInterface<IResolver> Resolver;
};

/* UActorComponent subclass of FInjectOnConstruction */
UCLASS()
class UNREALDITESTS_API UInjectComponent : public UActorComponent, public FInjectOnConstruction
{
    GENERATED_BODY()
public:
    UInjectComponent()
    {
        TryInitDependencies(this);
    }

    void InitDependencies(TScriptInterface<IResolver>&& InResolver)
    {
        Resolver = MoveTemp(InResolver);
    }

    UPROPERTY()
    TScriptInterface<IResolver> Resolver;
};

/* Actor with UInjectComponent */
UCLASS()
class UNREALDITESTS_API AInjectActorWithComponent : public AActor
{
    GENERATED_BODY()
public:
    AInjectActorWithComponent()
    {
        Component = CreateDefaultSubobject<UInjectComponent>(TEXT("Component"));
    }

    UPROPERTY()
    UInjectComponent* Component;
};

/* Returns specified Injector */
UCLASS()
class UTestInjectorProvider : public UObject, public IInjectorProvider
{
    GENERATED_BODY()
public:
    TScriptInterface<IInjector> GetInjector(UObject* InjectTarget) const override { return Injector; }

    UPROPERTY()
    TScriptInterface<IInjector> Injector;
};
