// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "DI/Impl/DependenciesRegistry.h"
#include "Misc/CoreMiscDefines.h"

namespace UnrealDI_Impl
{
    template <typename T>
    struct TExposeDependenciesHelper
    {
        TExposeDependenciesHelper()
        {
            FDependenciesRegistry::ExposeDependencies<T>();
        }
    };

#define EXPOSE_DEPENDENCIES_INTERNAL(Class) \
    UnrealDI_Impl::TExposeDependenciesHelper<Class> ANONYMOUS_VARIABLE(ExposeStruct_ ## Class);
}
