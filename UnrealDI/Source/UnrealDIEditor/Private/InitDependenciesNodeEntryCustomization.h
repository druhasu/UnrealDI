// Copyright Andrei Sudarikov. All Rights Reserved.

#pragma once

#include "IPropertyTypeCustomization.h"

class SComboButton;

class FInitDependenciesNodeEntryCustomization : public IPropertyTypeCustomization
{
public:
    using ThisClass = FInitDependenciesNodeEntryCustomization;

    static TSharedRef<IPropertyTypeCustomization> Make() { return MakeShared<FInitDependenciesNodeEntryCustomization>(); }

    void CustomizeHeader(TSharedRef<IPropertyHandle> InPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils) override;
    void CustomizeChildren(TSharedRef<IPropertyHandle> InPropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils) override {}

private:
    FText GetDependencyName() const;
    TSharedRef<SWidget> MakeDependencyClassPicker();
    TSharedRef<SWidget> GetClassPickerMenuContent();

    void OnClassPicked(UClass* InClass);

    TSharedPtr<IPropertyHandle> PropertyHandle;
    TSharedPtr<IPropertyHandle> DependencyTypePropertyHandle;

    TSharedPtr<SComboButton> ComboButton;
};
