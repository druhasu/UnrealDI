// Copyright Andrei Sudarikov. All Rights Reserved.

#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"

#include "K2Node_InitDependencies.h"
#include "InitDependenciesNodeDetails.h"
#include "InitDependenciesNodeEntryCustomization.h"

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
    }

    void ShutdownModule() override
    {
        FPropertyEditorModule* PropertyModule = FModuleManager::GetModulePtr<FPropertyEditorModule>("PropertyEditor");
        FBlueprintGraphModule* BlueprintGraphModule = FModuleManager::GetModulePtr<FBlueprintGraphModule>("BlueprintGraph");

        if (PropertyModule != nullptr)
        {
            PropertyModule->UnregisterCustomClassLayout(UK2Node_InitDependencies::StaticClass()->GetFName());
        }

        if (BlueprintGraphModule != nullptr)
        {
            BlueprintGraphModule->GetExtendedActionMenuFilters().RemoveAll([&](const FFilterDelegate& Delegate) { return Delegate.GetHandle() == FilterDelegateHandle; });
        }
    }

private:
    FDelegateHandle FilterDelegateHandle;
};

IMPLEMENT_MODULE(FUnrealDIEditorModule, UnrealDIEditor)
