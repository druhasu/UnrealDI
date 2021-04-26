// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "Containers/Map.h"
#include "Templates/SharedPointer.h"
#include "UObject/UObjectArray.h"

class UNREALDI_API FNewObjectInjector final : public FUObjectArray::FUObjectCreateListener
{
public:
    static FNewObjectInjector& Get() { return Instance; }

    void SetContainerForWorld(class UWorld* World, class UObjectContainer* Container);
    void RemoveContainerFromWorld(class UWorld* World);

protected:
    void NotifyUObjectCreated(const class UObjectBase* Object, int32 Index) override;
    void OnUObjectArrayShutdown() override {}

private:
    FNewObjectInjector() = default;

    static FNewObjectInjector Instance;

    TMap<UWorld*, UObjectContainer*> ContainerMap;
};