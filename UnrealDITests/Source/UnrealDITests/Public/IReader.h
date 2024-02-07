// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "UObject/Interface.h"
#include "IReader.generated.h"

UINTERFACE()
class UNREALDITESTS_API UReader : public UInterface
{
    GENERATED_BODY()
};

class UNREALDITESTS_API IReader
{
    GENERATED_BODY()

public:
    virtual FString Read() = 0;
};
