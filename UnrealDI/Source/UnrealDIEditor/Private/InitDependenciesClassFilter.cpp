// Copyright Andrei Sudarikov. All Rights Reserved.

#include "InitDependenciesClassFilter.h"
#include "InitDependenciesClassFilterSettings.h"
#include "Kismet/BlueprintFunctionLibrary.h"

bool FInitDependenciesClassFilter::IsClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const UClass* InClass, TSharedRef<class FClassViewerFilterFuncs> InFilterFuncs)
{
    if (InClass == nullptr)
    {
        return false;
    }

    // ignore some classes that do not make sense in this list
    if (IsBlacklistedClass(InClass))
    {
        return false;
    }

    const UInitDependenciesClassFilterSettings* Settings = GetDefault<UInitDependenciesClassFilterSettings>();

    if (Settings->bShowOnlyProjectClasses && !IsClassLocatedInCurrentProject(InClass))
    {
        return false;
    }

    if (Settings->bShowOnlyInterfaces && !InClass->IsChildOf<UInterface>())
    {
        return false;
    }

    return true;
}

bool FInitDependenciesClassFilter::IsUnloadedClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const TSharedRef<const class IUnloadedBlueprintData> InUnloadedClassData, TSharedRef<class FClassViewerFilterFuncs> InFilterFuncs)
{
    // blueprint classes should never be shown
    return false;
}

void FInitDependenciesClassFilter::GetFilterOptions(TArray<TSharedRef<FClassViewerFilterOption>>& OutFilterOptions)
{
    TSharedRef<FClassViewerFilterOption>& ShowOnlyProjectClassesOption = OutFilterOptions.Add_GetRef(MakeShared<FClassViewerFilterOption>());
    ShowOnlyProjectClassesOption->bEnabled = GetDefault<UInitDependenciesClassFilterSettings>()->bShowOnlyProjectClasses;
    ShowOnlyProjectClassesOption->LabelText = NSLOCTEXT("UnrealDI", "InitDependenciesFilter.ShowOnlyProjectClasses.Label", "Show only project classes");
    ShowOnlyProjectClassesOption->ToolTipText = NSLOCTEXT("UnrealDI", "InitDependenciesFilter.ShowOnlyProjectClasses.ToolTip", "When enabled, only classes in current project will be shown. Engine and Plugin classes will be omitted");
    ShowOnlyProjectClassesOption->OnOptionChanged.BindStatic([](bool bNewEnabled) { GetMutableDefault<UInitDependenciesClassFilterSettings>()->SetShowOnlyProjectClasses(bNewEnabled); });

    TSharedRef<FClassViewerFilterOption>& ShowImplementationsOption = OutFilterOptions.Add_GetRef(MakeShared<FClassViewerFilterOption>());
    ShowImplementationsOption->bEnabled = GetDefault<UInitDependenciesClassFilterSettings>()->bShowOnlyInterfaces;
    ShowImplementationsOption->LabelText = NSLOCTEXT("UnrealDI", "InitDependenciesFilter.ShowOnlyInterfaces.Label", "Show only interfaces");
    ShowImplementationsOption->ToolTipText = NSLOCTEXT("UnrealDI", "InitDependenciesFilter.ShowOnlyInterfaces.ToolTip", "When enabled, only interfaces will be shown");
    ShowImplementationsOption->OnOptionChanged.BindStatic([](bool bNewEnabled) { GetMutableDefault<UInitDependenciesClassFilterSettings>()->SetShowOnlyInterfaces(bNewEnabled); });
}

bool FInitDependenciesClassFilter::IsClassLocatedInCurrentProject(const UClass* InClass) const
{
    UPackage* ClassPackage = InClass->GetOuterUPackage();
    if (ClassPackage == nullptr)
    {
        return false;
    }

    FModuleStatus ModuleStatus;
    if (!FModuleManager::Get().QueryModule(FPackageName::GetShortFName(ClassPackage->GetFName()), ModuleStatus))
    {
        return false;
    }

    if (ModuleStatus.bIsGameModule)
    {
        // assume all Game modules belong to current project
        return true;
    }

    if (ModuleStatus.FilePath.Contains(FPaths::GameSourceDir()))
    {
        // this module is inside game Source directory
        return true;
    }

    return false;
}

bool FInitDependenciesClassFilter::IsBlacklistedClass(const UClass* InClass) const
{
    return InClass->IsChildOf(UBlueprintFunctionLibrary::StaticClass())
        || Cast<UBlueprintGeneratedClass>(InClass) != nullptr;
}
