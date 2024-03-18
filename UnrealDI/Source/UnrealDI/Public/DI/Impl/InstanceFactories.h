// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "DI/IInstanceFactory.h"
#include "InstanceFactories.generated.h"

UCLASS()
class UInstanceFactory_Object : public UObject, public IInstanceFactory
{
    GENERATED_BODY()

public:
    bool IsClassSupported(UClass* EffectiveClass) const override { return true; }
    UObject* Create(UObject* Outer, UClass* EffectiveClass) const override;
    void FinalizeCreation(UObject* Object) const override {}
};

UCLASS()
class UInstanceFactory_Actor : public UObject, public IInstanceFactory
{
    GENERATED_BODY()

public:
    bool IsClassSupported(UClass* EffectiveClass) const override;
    UObject* Create(UObject* Outer, UClass* EffectiveClass) const override;
    void FinalizeCreation(UObject* Object) const override;
};

UCLASS()
class UInstanceFactory_UserWidget : public UObject, public IInstanceFactory
{
    GENERATED_BODY()

public:
    bool IsClassSupported(UClass* EffectiveClass) const override;
    UObject* Create(UObject* Outer, UClass* EffectiveClass) const override;
    void FinalizeCreation(UObject* Object) const override {}
};
