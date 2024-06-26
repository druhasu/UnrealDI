// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "ClassViewerFilter.h"

/*
 * This class adds custom filters to Class picker used in InitDependencies node
 */
class FInitDependenciesClassFilter : public IClassViewerFilter
{
public:
    bool IsClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const UClass* InClass, TSharedRef< class FClassViewerFilterFuncs > InFilterFuncs) override;
    bool IsUnloadedClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const TSharedRef< const class IUnloadedBlueprintData > InUnloadedClassData, TSharedRef< class FClassViewerFilterFuncs > InFilterFuncs) override;
    void GetFilterOptions(TArray<TSharedRef<FClassViewerFilterOption>>& OutFilterOptions) override;

private:
    bool IsClassLocatedInCurrentProject(const UClass* InClass) const;
    bool IsBlacklistedClass(const UClass* InClass) const;
};
