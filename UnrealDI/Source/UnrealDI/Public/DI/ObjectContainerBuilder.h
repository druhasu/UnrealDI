// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "Templates/Function.h"
#include "Containers/Array.h"
#include "DI/Impl/RegistrationConfigurator_ForType.h"
#include "DI/Impl/RegistrationConfigurator_ForInstance.h"
#include "DI/Impl/RegistrationConfigurator_ForFactory.h"

class UObject;
class UObjectContainer;
class UGameInstance;
class UWorld;

/*
 * Helper class to simplify construction of UObjectContainer.
 * Call appropriate Register...() method for everything that should be in the container.
 * After that call Build() and provide valid Outer.
 * If no Outer is given you won't be able to construct AActor and UUserWidget subclasses using default factories.
 * Call BuildNested() to create a nested container - this one will be able to resolve types registered in it and in its parent
 */
class UNREALDI_API FObjectContainerBuilder
{
public:
    /*
     * Adds registration for type TObject using default factory.
     * By default objects are handled by Transient lifetime.
     */
    template<typename TObject>
    UnrealDI_Impl::TRegistrationConfigurator_ForType<TObject>& RegisterType()
    {
        return AddConfigurator< UnrealDI_Impl::TRegistrationConfigurator_ForType< TObject > >();
    }

    /*
     * Adds registration for type TObject using provided instance.
     */
    template<typename TObject>
    UnrealDI_Impl::TRegistrationConfigurator_ForInstance<TObject>& RegisterInstance(TObject* Instance)
    {
        return AddConfigurator< UnrealDI_Impl::TRegistrationConfigurator_ForInstance< TObject > >(Instance);
    }

    /*
     * Adds registration for type TObject using provided Factory function.
     * Factory just have to construct object. Injection will be performed later by the container.
     */
    template<typename TObject>
    UnrealDI_Impl::TRegistrationConfigurator_ForFactory<TObject>& RegisterFactory(TFunction< TObject* () > Factory)
    {
        return AddConfigurator< UnrealDI_Impl::TRegistrationConfigurator_ForFactory< TObject > >(Factory);
    }

    /* 
     * Builds a container from all registered types.
     * Outer is used to access current UWorld. If you are creating application-wide container use UGameInstance as an Outer.
     */
    UObjectContainer* Build(UObject* Outer = nullptr);

    /*
     * Builds nested container from all registered types.
     * Nested container can access all registrations from Parent if required.
     */
    UObjectContainer* BuildNested(UObjectContainer& Parent);

private:
    template<typename TConfigurator, typename... TArgs>
    TConfigurator& AddConfigurator(TArgs... Args)
    {
        TSharedRef<TConfigurator> Ret = MakeShared<TConfigurator>(Args...);
        Registrations.Emplace(StaticCastSharedRef< UnrealDI_Impl::FRegistrationConfiguratorBase >(Ret));
        return *Ret;
    }

    void AddRegistrationsToContainer(UObjectContainer* Container);

    TArray<TSharedRef<UnrealDI_Impl::FRegistrationConfiguratorBase>> Registrations;
};