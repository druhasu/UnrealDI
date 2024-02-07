// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "IReader.h"
#include "GameFramework/Actor.h"
#include "Blueprint/UserWidget.h"
#include "MockClasses_EmptyDependencies.generated.h"

UCLASS()
class UObjectNoDeps : public UObject
{
    GENERATED_BODY()

public:
    void InitDependencies()
    {
        bWasCalled = true;
    }

    bool bWasCalled = false;
};

UCLASS()
class AActorNoDeps : public AActor
{
    GENERATED_BODY()

public:
    void InitDependencies()
    {
        bWasCalled = true;
    }

    bool bWasCalled = false;
};

UCLASS()
class UWidgetNoDeps : public UUserWidget
{
    GENERATED_BODY()

public:
    void InitDependencies()
    {
        bWasCalled = true;
    }

    bool bWasCalled = false;
};