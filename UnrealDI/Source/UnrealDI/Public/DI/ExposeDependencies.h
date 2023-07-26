// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "DI/Impl/DependenciesRegistry.h"
#include "Misc/CoreMiscDefines.h"

namespace UnrealDI_Impl
{
    template <typename T>
    struct TExposeDependenciesHelper
    {
        static uint8 Register;
    };

    template <typename T>
    uint8 TExposeDependenciesHelper<T>::Register = FDependenciesRegistry::ExposeDependencies<T>();
}

/*
 * Exposes InitDependencies signature of given Class for DI Container.
 * Any DI Container may use this exposed signature to Resolve and Inject objects of this class.
 * You can use IInjector on objects of Class registered with this macro
 * 
 * Usage Example:
 * 
 *   #include "MyClass.h"
 *   #include "SomeDependency.h"
 *   #include "DI/ExposeDependencies.h"
 *   
 *   EXPOSE_DEPENDENCIES(UMyClass);
 *   
 *   void UMyClass::InitDependencies( USomeDependency* InDependency )
 *   {
 *       // Do something with InDependency
 *   }
 * 
 * In some other place:
 * 
 *   UMyClass* MyClassInstance;
 *   Injector->Inject(MyClassInstance); // will call InitDependencies as expected
 * 
 */
#define EXPOSE_DEPENDENCIES(Class) \
    struct ANONYMOUS_VARIABLE(ExposeStruct) \
    { \
        static void Dummy() \
        { \
            const uint8& ExposedValue = UnrealDI_Impl::TExposeDependenciesHelper<Class>::Register; \
        } \
    }
