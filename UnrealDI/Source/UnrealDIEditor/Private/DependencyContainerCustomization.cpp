#include "DependencyContainerCustomization.h"

#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "EditorClassUtils.h"
#include "IDetailChildrenBuilder.h"
#include "PropertyCustomizationHelpers.h"
#include "Di/Impl/DependencyContainerProxy.h"

void FDependencyContainerCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle,
														FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	StructHandle = PropertyHandle;
	TypeHandle = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FDependencyContainer, Object));
	ValueHandle = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FDependencyContainer, RegistrationType));
	
	HeaderRow
	.ValueContent()
	[
		ValueHandle->CreatePropertyValueWidget()
	]
	.NameContent()
	.MinDesiredWidth(200)
	.MaxDesiredWidth(4096)
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot().AutoWidth().Padding(2, 0, 2, 0)
		[
			SNew(SClassPropertyEntryBox)
			.AllowNone(true)
			.MetaClass(UObject::StaticClass())
			.AllowAbstract(false)
			.ShowTreeView(true)
			.OnSetClass(this, &FDependencyContainerCustomization::HandleNewEntryClassSelected)
			.SelectedClass(this, &FDependencyContainerCustomization::GetSelectedEntryClass)
		]
	];
}

void FDependencyContainerCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	if (PropertyHandle->IsValidHandle())
	{
		const TSharedPtr<IPropertyHandle> ChildHandle = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FDependencyContainer, RegistrationType));
		if (const TSharedPtr<IPropertyHandleArray> Array = ChildHandle->AsArray(); Array.IsValid())
		{
			Array->SetOnNumElementsChanged(FSimpleDelegate::CreateSPLambda(this, [&ChildBuilder]()
			{
				ChildBuilder.GetParentCategory().GetParentLayout().ForceRefreshDetails();
			}));
			uint32 Elements;
			Array->GetNumElements(Elements);
			for (uint32 i = 0; i < Elements; i++)
			{
				TSharedRef<IPropertyHandle> ElementHandle = Array->GetElement(i);
				GenerateArrayElement(ElementHandle, i, ChildBuilder);
			}
		}
	}
}

void FDependencyContainerCustomization::GetTypeOptions(TArray<TSharedPtr<FString>>& Names, TArray<TSharedPtr<SToolTip>>& Tooltips, TArray<bool>& Enabled) const
{
	if (ClassPtr.IsValid())
	{
		UClass* Copy = ClassPtr.Get();
		while (Copy != UObject::StaticClass())
		{
			for (const FImplementedInterface& Interface : Copy->Interfaces)
			{
				Names.Add(MakeShared<FString>(Interface.Class.GetName()));
			}
			Copy = Copy->GetSuperClass();
		}
	}
}

void FDependencyContainerCustomization::GenerateArrayElement(TSharedRef<IPropertyHandle> ElementHandle, int32 Index, IDetailChildrenBuilder& ChildrenBuilder)
{
	FPropertyComboBoxArgs Args;
	Args.PropertyHandle = ElementHandle;
	Args.OnGetStrings = FOnGetPropertyComboBoxStrings::CreateSP(this, &FDependencyContainerCustomization::GetTypeOptions);
				
	IDetailPropertyRow& Builder = ChildrenBuilder.AddProperty(ElementHandle);
	Builder.CustomWidget()
	.NameContent()
	[
		ElementHandle->CreatePropertyNameWidget()
	]
	.ValueContent()
	.MaxDesiredWidth(500.0f)
	.MinDesiredWidth(100.0f)
	[
		PropertyCustomizationHelpers::MakePropertyComboBox(Args)
	];
}

const UClass* FDependencyContainerCustomization::GetSelectedEntryClass() const
{
	FString ClassName;
	TypeHandle->GetValueAsFormattedString(ClassName);
	
	const UClass* Class = ClassPtr.Get();
	if(!Class || Class->GetPathName() != ClassName)
	{
		Class = FEditorClassUtils::GetClassFromString(ClassName);
		ClassPtr = MakeWeakObjectPtr(const_cast<UClass*>(Class));
	}
	return Class;
}

void FDependencyContainerCustomization::HandleNewEntryClassSelected(const UClass* NewEntryClass)
{
	if (TypeHandle->SetValueFromFormattedString((NewEntryClass) ? NewEntryClass->GetPathName() : "None") == FPropertyAccess::Result::Success)
	{
		ClassPtr = MakeWeakObjectPtr(const_cast<UClass*>(NewEntryClass));
	}
}
