// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "UObject/Interface.h"
#include "DI/ObjectContainer.h"
#include "DI/Impl/InitDependenciesInvoker.h"
#include "DI/Impl/InitMethodTypologyDeducer.h"

#include "DI/Impl/ArgumentPack.h"
#include "IInjectOnConstruction.generated.h"

UINTERFACE()
class UNREALDI_API UInjectOnConstruction : public UInterface
{
    GENERATED_BODY()
};

/*
 * Marker Interface for objects that need injection from World-bound container when the object is constructed.
 * You must call TryInitDependencies(this) in constructor of derived class.
 * You also need to register container with you World using FNewObjectInjector::SetContainerForWorld()
 */
class UNREALDI_API IInjectOnConstruction
{
    GENERATED_BODY()

protected:
    IInjectOnConstruction()
    {
        // if this condition is false, we better crash here
        // if we continue, then we may call InitDependencies with wrong container which may lead to a crash later
        // but for that crash it will be almost impossible to find the root cause
        checkf(NextExpectedObject == this || NextExpectedObject == nullptr,
            TEXT("Unexpected NextObject. Make sure previous object '%s' of type '%s' calls TryInitDependencies in its constructor."),
            *NextUObject->GetFName().ToString(), *NextUObject->GetClass()->GetName());
    }

    template <typename T>
    void TryInitDependencies(T* Self)
    {
        if (NextConstructorContainer)
        {
            using Invoker = UnrealDI_Impl::TInitDependenciesInvoker<T, UnrealDI_Impl::TInitMethodTypologyDeducer< T >>;

            Invoker::Invoke(Self, *NextConstructorContainer);

            NextConstructorContainer = nullptr;
            NextExpectedObject = nullptr;
            NextUObject = nullptr;
        }
    }

private:
    friend class FNewObjectInjector;

    static UObjectContainer* NextConstructorContainer;

    // fields to help find missing calls to TryCallInitDependencies
    static IInjectOnConstruction* NextExpectedObject;
    static const UObjectBase* NextUObject;
};
