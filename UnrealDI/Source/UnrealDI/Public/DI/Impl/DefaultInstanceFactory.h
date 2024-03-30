// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "DI/IInstanceFactory.h"
#include "DefaultInstanceFactory.generated.h"

UCLASS(HideDropDown)
class UNREALDI_API UDefaultInstanceFactory : public UObject, public IInstanceFactory
{
    GENERATED_BODY()

public:
    bool IsClassSupported(UClass* EffectiveClass) const override { return true; }
    UObject* Create(UObject* Outer, UClass* EffectiveClass) const override;
    void FinalizeCreation(UObject* Object) const override;
};
