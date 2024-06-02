// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "DI/DependencyResolver.h"
#include "DI/IResolver.h"

template <typename T>
struct TTestDependency
{
    T* Object;
};

template <typename T>
struct TDependencyResolver< TTestDependency<T> >
{
    static TTestDependency<T> Resolve(const IResolver& Resolver)
    {
        return { Cast<T>(Resolver.Resolve(T::StaticClass())) };
    }
};
