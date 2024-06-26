// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "IReader.h"
#include "MockReader.generated.h"

/* Implements IReader interface */
UCLASS(Blueprintable)
class UNREALDITESTS_API UMockReader : public UObject, public IReader
{
    GENERATED_BODY()

public:
    FString Read() override { return NextValue; }

    FString NextValue;
};
