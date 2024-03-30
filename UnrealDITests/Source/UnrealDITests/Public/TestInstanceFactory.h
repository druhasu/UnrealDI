// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "DI/IInstanceFactory.h"
#include "TestInstanceFactory.generated.h"

class UTestInstanceFactory;

UCLASS()
class UTestInstanceFactoryObject : public UObject
{
    GENERATED_BODY()

public:
    UTestInstanceFactory* CreatedBy = nullptr;
    bool bFinalizeCalled = false;
};

UCLASS()
class UTestInstanceFactoryUnsupportedObject : public UObject
{
    GENERATED_BODY()
};

UCLASS(HideDropDown)
class UTestInstanceFactory : public UObject, public IInstanceFactory
{
    GENERATED_BODY()

public:
    bool IsClassSupported(UClass* EffectiveClass) const override
    {
        return EffectiveClass->IsChildOf(BaseClass);
    }

    UObject* Create(UObject* Outer, UClass* EffectiveClass) const override
    {
        UObject* Result = NewObject<UObject>(Outer, EffectiveClass);
        if (UTestInstanceFactoryObject* Obj = Cast<UTestInstanceFactoryObject>(Result))
        {
            Obj->CreatedBy = const_cast<UTestInstanceFactory*>(this);
        }
        CreatedObjects.Add(Result);
        return Result;
    }

    void FinalizeCreation(UObject* Object) const override
    {
        if (UTestInstanceFactoryObject* Obj = Cast<UTestInstanceFactoryObject>(Object))
        {
            Obj->bFinalizeCalled = true;
        }
    }

    UClass* BaseClass = UTestInstanceFactoryObject::StaticClass();

    mutable TArray<UObject*> CreatedObjects;
};
