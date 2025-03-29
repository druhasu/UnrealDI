// Copyright Andrei Sudarikov. All Rights Reserved.

#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "KismetCompiler.h"

#include "K2Node_InitDependencies.h"
#include "InitDependenciesNodeDetails.h"
#include "InitDependenciesNodeEntryCustomization.h"
#include "DI/Impl/DependenciesRegistry.h"

class FUnrealDIEditorModule : public IModuleInterface
{
public:
    using ThisClass = FUnrealDIEditorModule;
    using FFilterDelegate = FBlueprintGraphModule::FActionMenuRejectionTest;

    void StartupModule() override
    {
        FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
        FBlueprintGraphModule& BlueprintGraphModule = FModuleManager::LoadModuleChecked<FBlueprintGraphModule>("BlueprintGraph");

        PropertyModule.RegisterCustomClassLayout(UK2Node_InitDependencies::StaticClass()->GetFName(), FOnGetDetailCustomizationInstance::CreateStatic(&FInitDependenciesNodeDetails::Make));
        PropertyModule.RegisterCustomPropertyTypeLayout(FInitDependenciesNodeEntry::StaticStruct()->GetFName(), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FInitDependenciesNodeEntryCustomization::Make));

        FFilterDelegate Dlg = FFilterDelegate::CreateStatic(&UK2Node_InitDependencies::FilterAction);
        FilterDelegateHandle = Dlg.GetHandle();
        BlueprintGraphModule.GetExtendedActionMenuFilters().Add(Dlg);

        FKismetCompilerContext::OnPostCompile.AddRaw(this, &ThisClass::OnBlueprintCompiled);
    }

    void ShutdownModule() override
    {
        FPropertyEditorModule* PropertyModule = FModuleManager::GetModulePtr<FPropertyEditorModule>("PropertyEditor");
        FBlueprintGraphModule* BlueprintGraphModule = FModuleManager::GetModulePtr<FBlueprintGraphModule>("BlueprintGraph");

        if (PropertyModule != nullptr)
        {
            PropertyModule->UnregisterCustomClassLayout(UK2Node_InitDependencies::StaticClass()->GetFName());
            PropertyModule->UnregisterCustomPropertyTypeLayout(FInitDependenciesNodeEntry::StaticStruct()->GetFName());
        }

        if (BlueprintGraphModule != nullptr)
        {
            BlueprintGraphModule->GetExtendedActionMenuFilters().RemoveAll([&](const FFilterDelegate& Delegate) { return Delegate.GetHandle() == FilterDelegateHandle; });
        }

        FKismetCompilerContext::OnPostCompile.RemoveAll(this);
    }

    void OnBlueprintCompiled()
    {
        // once blueprint is compiled, out cache of InitDependency functions is no longer valid
        // it's not easy to determine which class was compiled, so we drop whole cache
        // during edit time it is acceptable to have small performance hit, so it is not a big deal
        UnrealDI_Impl::FDependenciesRegistry::ClearBlueprintInitFunctionsCache();
    }

private:
    FDelegateHandle FilterDelegateHandle;
};

IMPLEMENT_MODULE(FUnrealDIEditorModule, UnrealDIEditor)
