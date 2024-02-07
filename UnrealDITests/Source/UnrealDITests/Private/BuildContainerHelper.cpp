// Copyright Andrei Sudarikov. All Rights Reserved.

#include "BuildContainerHelper.h"

#include "MockReader.h"
#include "DI/ObjectContainerBuilder.h"
#include "DI/ObjectContainer.h"

UObjectContainer* FBuildContainerHelper::Build(UObject* Outer, TFunctionRef<void(FObjectContainerBuilder&)> AddRegistrations)
{
    FObjectContainerBuilder Builder;
    Builder.RegisterType<UMockReader>().As<IReader>().AsSelf();
    AddRegistrations(Builder);

    return Builder.Build(Outer);
}
