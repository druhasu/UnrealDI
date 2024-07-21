// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "MockClasses.h"
#include "IBlueprintDependencyInterface.h"
#include "MockClasses_BlueprintInitDependencies.generated.h"

UCLASS()
class UBlueprintDependencyInterfaceImpl : public UObject, public IBlueprintDependencyInterface
{
    GENERATED_BODY()

public:
    FString GetValue() const override { return TEXT("Test String"); }
};

UCLASS(BlueprintType)
class UBlueprintDependencyObject : public UObject
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    FString GetValue() const { return TEXT("Test String"); }
};

UCLASS(Blueprintable)
class UTestBlueprintInitDependencies : public UObject
{
    GENERATED_BODY()

public:
    void InitDependencies(TScriptInterface<IReader>&& InReader)
    {
        Reader = MoveTemp(InReader);
    }

    UPROPERTY(BlueprintReadWrite)
    TScriptInterface<IReader> Reader;

    UPROPERTY(BlueprintReadWrite)
    TScriptInterface<IBlueprintDependencyInterface> DependencyInterface;

    UPROPERTY(BlueprintReadWrite)
    TObjectPtr<UBlueprintDependencyObject> DependencyObject;
};

UCLASS(Blueprintable)
class UTestTryInitDependencies : public UObject
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintImplementableEvent)
    void CallTryInitDependencies();

    UPROPERTY(BlueprintReadWrite)
    TScriptInterface<IBlueprintDependencyInterface> DependencyInterface;
};
