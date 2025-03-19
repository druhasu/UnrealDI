// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IPropertyTypeCustomization.h"

/**
 * 
 */
class FDependencyContainerCustomization : public IPropertyTypeCustomization
{
	
public:
	
	static TSharedRef<IPropertyTypeCustomization> MakeInstance()
	{
		return MakeShareable(new FDependencyContainerCustomization);
	}
	
	//~ Begin IPropertyTypeCustomization Interface
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils) override;
	//~ End IPropertyTypeCustomization Interface

private:

	void GetTypeOptions(TArray<TSharedPtr<FString>>& Names, TArray<TSharedPtr<SToolTip>>& Tooltips, TArray<bool>& Enabled) const;
	void GenerateArrayElement(TSharedRef<IPropertyHandle> ElementHandle, int32 Index, IDetailChildrenBuilder& ChildrenBuilder);
	const UClass* GetSelectedEntryClass() const;
	void HandleNewEntryClassSelected(const UClass* NewEntryClass);

	mutable TWeakObjectPtr<UClass> ClassPtr;
	
	TSharedPtr<IPropertyHandle> StructHandle;
	TSharedPtr<IPropertyHandle> TypeHandle;
	TSharedPtr<IPropertyHandle> ValueHandle;
};
