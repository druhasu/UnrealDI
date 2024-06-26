// Copyright Andrei Sudarikov. All Rights Reserved.

#include "InitDependenciesNodeEntryCustomization.h"
#include "DetailWidgetRow.h"
#include "ClassViewerModule.h"
#include "InitDependenciesClassFilter.h"
#include "Widgets/Input/SComboButton.h"

#include "K2Node_InitDependencies.h"

void FInitDependenciesNodeEntryCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> InPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
    PropertyHandle = InPropertyHandle;
    DependencyTypePropertyHandle = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FInitDependenciesNodeEntry, DependencyType));

    HeaderRow.NameContent()
    [
        SNew(STextBlock)
        .Font(CustomizationUtils.GetRegularFont())
        .Text(this, &ThisClass::GetDependencyName)
    ];

    HeaderRow.ValueContent()
    [
        MakeDependencyClassPicker()
    ];
}

FText FInitDependenciesNodeEntryCustomization::GetDependencyName() const
{
    UObject* Object = nullptr;
    DependencyTypePropertyHandle->GetValue(Object);

    return FText::FromString(GetNameSafe(Object));
}

TSharedRef<SWidget> FInitDependenciesNodeEntryCustomization::MakeDependencyClassPicker()
{
    return SAssignNew(ComboButton, SComboButton)
        .OnGetMenuContent(this, &ThisClass::GetClassPickerMenuContent)
        .ButtonContent()
        [
            SNew(STextBlock)
            .Font(IPropertyTypeCustomizationUtils::GetRegularFont())
            .Text(this, &ThisClass::GetDependencyName)
        ];
}

TSharedRef<SWidget> FInitDependenciesNodeEntryCustomization::GetClassPickerMenuContent()
{
    FClassViewerInitializationOptions ClassViewerOptions;
    ClassViewerOptions.Mode = EClassViewerMode::ClassPicker;
    ClassViewerOptions.bShowBackgroundBorder = false;
    ClassViewerOptions.ClassFilters.Add(MakeShared<FInitDependenciesClassFilter>());

    return SNew(SBox)
        .WidthOverride(280)
        [
            SNew(SVerticalBox)
            +SVerticalBox::Slot()
            .AutoHeight()
            .MaxHeight(500)
            [
                FModuleManager::LoadModuleChecked<FClassViewerModule>("ClassViewer").CreateClassViewer(ClassViewerOptions, FOnClassPicked::CreateRaw(this, &ThisClass::OnClassPicked))
            ]
        ];
}

void FInitDependenciesNodeEntryCustomization::OnClassPicked(UClass* InClass)
{
    DependencyTypePropertyHandle->SetValue(InClass);
    ComboButton->SetIsOpen(false);
}
