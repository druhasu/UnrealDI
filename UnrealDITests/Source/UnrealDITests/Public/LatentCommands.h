// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "Tests/AutomationCommon.h"

class FRunGC : public IAutomationLatentCommand
{
public:
    FRunGC()
    {
        Counter = GLastGCFrame;
        GEngine->ForceGarbageCollection(true);
    }

    bool Update() override
    {
        return GLastGCFrame > Counter;
    }

private:
    int32 Counter;
};