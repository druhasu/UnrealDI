// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "Templates/Function.h"
#include "Containers/Array.h"
#include "Delegates/IntegerSequence.h"
#include "DI/Impl/RegistrationConfigurator_ForType.h"
#include "DI/Impl/RegistrationConfigurator_ForInstance.h"
#include "DI/Impl/RegistrationConfigurator_ForFactory.h"

class UObject;
class UObjectContainer;
class UGameInstance;
class UWorld;

class UNREALDI_API FObjectContainerBuilder
{
public:
    template<typename TObject>
    UnrealDI_Impl::TRegistrationConfigurator_ForType<TObject>& RegisterType()
    {
        return AddConfigurator< UnrealDI_Impl::TRegistrationConfigurator_ForType< TObject > >();
    }

    template<typename TObject>
    UnrealDI_Impl::TRegistrationConfigurator_ForInstance<TObject>& RegisterInstance(TObject* Instance)
    {
        return AddConfigurator< UnrealDI_Impl::TRegistrationConfigurator_ForInstance< TObject > >(Instance);
    }

    template<typename TObject>
    UnrealDI_Impl::TRegistrationConfigurator_ForFactory<TObject>& RegisterFactory(TFunction< TObject* () > Factory)
    {
        return AddConfigurator< UnrealDI_Impl::TRegistrationConfigurator_ForFactory< TObject > >(Factory);
    }

    UObjectContainer* Build(UObject* Outer = nullptr);
    UObjectContainer* BuildNested(UObjectContainer& Parent);

private:
    template<typename TConfigurator, typename... TArgs>
    TConfigurator& AddConfigurator(TArgs... Args)
    {
        TSharedRef<TConfigurator> Ret = MakeShared<TConfigurator>(Args...);
        Registrations.Emplace(StaticCastSharedRef< UnrealDI_Impl::FRegistrationConfiguratorBase >(Ret));
        return *Ret;
    }

    TArray<TSharedRef<UnrealDI_Impl::FRegistrationConfiguratorBase>> Registrations;
};